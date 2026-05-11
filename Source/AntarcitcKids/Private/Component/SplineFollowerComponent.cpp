// SplineFollowerComponent.cpp

#include "Component/SplineFollowerComponent.h"
#include "AntarcitcKids.h"
#include "CityVehiclePawn.h"
#include "EngineUtils.h"

// UE Landscape Spline 시스템 관련 클래스들 포함
#include "LandscapeSplineActor.h"
#include "LandscapeSplineControlPoint.h"
#include "LandscapeSplineSegment.h"
#include "LandscapeSplinesComponent.h"

// 커스텀 로그 카테고리를 정의
DEFINE_LOG_CATEGORY_STATIC(LogPathFollowingComponent, Log, All);

USplineFollowerComponent::USplineFollowerComponent()
	: MaxSpeed(3000.f)
	, MinSpeed(400.f)
	, ThrottleGain(0.002f)
	, DecelRate(0.5f)
	, AccelRate(1.f)
	, LookAheadBase(1500.f)
	, LookAheadSpeedFactor(0.3f)
	, MaxYawDelta(40.f)
	, HeadingWeight(0.7f)
	, CrosstrackGain(0.0015f)
	, LateralFriction(0.8f)
	, CurvatureSampleSpan(50.f)
	, BrakePreviewDist(5000.f)
	, StartControlPoint(nullptr)
	, ResampleSpacing(50.f)
	, CurrentPointIndex(0)
	, bClosedLoop(false)
	, SmoothedTargetSpeed(0.f)
{
	PrimaryComponentTick.bCanEverTick = true;
	
	// BuildPath() 완료 후, 경로가 준비되면 Tick 활성화되도록 처음엔 비활성화하기
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USplineFollowerComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerPawn = Cast<ACityVehiclePawn>(GetOwner());
	BuildPath();
}

void USplineFollowerComponent::BuildPath()
{
	if (OwnerPawn.IsValid() == false)
	{
		UE_LOG(LogPathFollowingComponent, Error, TEXT("Failed to cache OwnerPawn."));
		return;
	}
	
	// 이전 경로 데이터 초기화 (.Reset() : 메모리는 유지하여 재할당 비용 절약)
	PathPoints.Reset();
	bClosedLoop = false;


	// ── 1. 월드에서 LandscapeSplineActor 탐색 ──────────────────────────────
    // 씬에 배치된 ALandscapeSplineActor를 순회하여 첫 번째 유효한 컴포넌트를 가져옴
    
	ULandscapeSplinesComponent* SplinesComp = nullptr;
	
	for (TActorIterator<ALandscapeSplineActor> It(GetWorld()); It; ++It)
	{
		// GetWorld()에서 받은 UWorld 객체를 직접 탐색
		// 컨테이너 이터레이터와 문법이 다른 이유 >>
				// UWorld는 모든 액터를 내부 레벨 구조에 분산 보관하고 있어서, 컨테이너처럼 단순히 begin()/end()로 꺼낼 수 없음
				// TActorIterator: GetWorld()를 받아서, 월드 내부의 레벨과 액터 목록을 직접 뒤지면서 템플릿 타입에 해당하는 것만 필터링해줌
						// World->GetLevels() 순회
						// 각 Level->Actors 순회
						// IsA<ALandscapeSplineActor>() 필터링
		SplinesComp = It->GetSplinesComponent();
		
		// 월드에 하나만 있다는 가정 하에, 찾으면 break
		if (SplinesComp) break;
	}
	
	if (!SplinesComp)
	{
		UE_LOG(LogAntarcitcKids, Warning, TEXT("SplineFollower: No ALandscapeSplineActor found"));
		return;
	}
	
	// FTransform : 위치/회전/크기 세 가지 데이터를 담은 구조체
	// 내부 함수 : 스플라인의 로컬→월드 변환 및 역변환을 가능하게 함
			// ToWorld.TransformPosition(LocalPos)   // 위치 변환 (이동 + 회전 + 스케일)
			// ToWorld.TransformVector(LocalDir)     // 방향 변환 (회전 + 스케일, 이동 제외)
			// ToWorld.TransformRotation(LocalRot)   // 회전만 변환
	const FTransform ToWorld = SplinesComp->GetComponentTransform();
	
	// 차량의 현재 위치
	const FVector VehicleLoc = OwnerPawn->GetActorLocation();
	
	
	// ── 2. StartCP 확정 ───────────────────────────────────────────────────
	// 에디터에서 직접 지정한 StartControlPoint를 사용
	// 역방향 탐색을 제거함으로써 세그먼트 Connections[0]/[1] 방향 혼용 문제 원천 차단
	// (세그먼트를 역방향으로 그렸을 때 시작점을 잘못 추정하던 버그 제거)
	
	if (!StartControlPoint)
	{
		UE_LOG(LogAntarcitcKids, Warning, TEXT("SplineFollower: StartControlPoint is not set"));
		return;
	}
	
	ULandscapeSplineControlPoint* StartCP = StartControlPoint.Get();
	

	// ── 3. 순방향 탐색: PathPoints 수집 ──────────────────────────────────
    // StartCP에서 출발하여 세그먼트를 따라 포인트를 순서대로 수집
    // bReversed : 세그먼트마다 포인트 순서를 보정하기 위해 필요
		    // Connections[0]/[1] 은 세그먼트 생성 방향에 따라 결정되므로
				// 특정 인덱스를 "항상 upstream/downstream" 으로 가정하면 안 됨
		    // CP_Start ── Seg1 ── CP_A ── Seg2 ── CP_End
			//            [0][1]          [1][0]
			//                               ↑ Seg2는 반대로 연결됨
    
	//Visited.Reset();
	TSet<ULandscapeSplineSegment*> Visited;
	
	ULandscapeSplineControlPoint* CurCP = StartCP;
	
	while (true)
	{
		ULandscapeSplineSegment* Seg = nullptr;
		for (const auto& Conn : CurCP->ConnectedSegments)
		{
			if (!Visited.Contains(Conn.Segment)) { Seg = Conn.Segment; break; }
		}
		
		// while루프 탈출
		if (!Seg) break; // 순방향으로 갈 세그먼트가 없음 = 체인의 끝점에 도달
		
		Visited.Add(Seg);
		
		// CurCP가 Connections[1](끝점)이면 이 세그먼트는 역방향으로 그려진 것
		// → 포인트를 역순으로 읽어서 진행 방향에 맞게 보정
		const bool bReversed = (Seg->Connections[1].ControlPoint == CurCP);
		
		// ULandscapeSplineSegment::GetPoints()
		// 'UE가 두 CP 사이를 내부적으로 보간한 점들'의 배열
		// CP_A ──────────────────────────── CP_B
		//   •  •     •  •  •     •     •  •  •
		//   ↑ 
		//   Pts
    
		// Seg1의 Pts:  •     •  •  [•]
		// Seg2의 Pts:              [•]  •     •  •
		//                          ↑ 같은 점이 두 번, 중복이므로 스킵
    
	    // FLandscapeSplineInterpPoint는 스플라인 위의 한 점에 대한 정보를 담은 구조체
	    // FVector 타입인 Center, Left, Right, FalloffLeft, FalloffRight 중 
	    // 차량 경로 추종 목적을 가진 Center 값 이용 예정
		const TArray<FLandscapeSplineInterpPoint>& Pts = Seg->GetPoints();

		if (Pts.Num() >= 2) // Pts에는 양 끝 CP가 포함이라 2개 이상!
		{
			// PathPoints.Num() == 0 이라면, 지금 첫번째 세그먼트라는 뜻이니까 스킵 안 함
			const bool bSkipFirst = PathPoints.Num() > 0;
			if (!bReversed)
				for (int32 i = (bSkipFirst ? 1 : 0); i < Pts.Num(); ++i)
					PathPoints.Add(ToWorld.TransformPosition(Pts[i].Center));
			else
				for (int32 i = Pts.Num() - 1 - (bSkipFirst ? 1 : 0); i >= 0; --i)
					PathPoints.Add(ToWorld.TransformPosition(Pts[i].Center));
		}

		ULandscapeSplineControlPoint* NextCP = bReversed
			? Seg->Connections[0].ControlPoint : Seg->Connections[1].ControlPoint;
		
		if (NextCP == StartCP) { bClosedLoop = true; break; } // 다음 CP가 StartCP이면 닫힌 루프 확정
		
		CurCP = NextCP;
	}

	if (PathPoints.Num() < 3)
	{
		UE_LOG(LogAntarcitcKids, Warning, TEXT("SplineFollower: Too few path points (%d)"), PathPoints.Num());
		return;
	}


	// ── 4. Catmull-Rom 재샘플링으로 경로를 부드럽게 보간 ─────────────────
		// 이후, GetPointAhead()나 곡률 계산(EstimateCurvature)은 일정한 간격을 가정하고 동작하기 때문에 재샘플링이 필요
		// GetPoints()    → 점 위치는 맞지만 간격 불규칙, 세그먼트 경계에서 꺾임
		// Catmull-Rom    → 일정 간격(ResampleSpacing), 세그먼트 경계도 부드럽게 연결
	ResampleCatmullRom();


	// ── 5. 차량 위치와 가장 가까운 포인트를 시작 인덱스로 설정 ──────────
		// 차량이 경로 어디쯤에 있는지 찾기
	float BestSq = TNumericLimits<float>::Max();
	CurrentPointIndex = 0;
	for (int32 i = 0; i < PathPoints.Num(); ++i)
	{
		// 비교 목적이므로 제곱근 불필요, 성능 절약
		const float Sq = FVector::DistSquared(PathPoints[i], VehicleLoc);
		if (Sq < BestSq) { BestSq = Sq; CurrentPointIndex = i; }
	}

	SmoothedTargetSpeed = MaxSpeed;
	SetComponentTickEnabled(true); // 경로 준비 완료 → Tick 활성화

	UE_LOG(LogAntarcitcKids, Log, TEXT("SplineFollower: %d pts, loop=%s, start=%d"),
		PathPoints.Num(), bClosedLoop ? TEXT("Y") : TEXT("N"), CurrentPointIndex);
}

static FVector EvalCatmullRom(const FVector& P0, const FVector& P1,
							   const FVector& P2, const FVector& P3, float T)
{
	// Catmull-Rom = 3차 에르미트 보간 + "접선은 앞뒤 포인트 중간 방향" 규칙
	// T=0.0 → P1 위치
	// T=0.5 → P1~P2 중간 (곡선 반영)
	// T=1.0 → P2 위치
	const float T2 = T * T, T3 = T2 * T;
	return 0.5f * (
		(2.f * P1) +                                    // T=0일 때 P1에서 시작
		(-P0 + P2) * T +                                // P1에서의 접선 방향 (기울기)
		(2.f * P0 - 5.f * P1 + 4.f * P2 - P3) * T2 +    // 곡률 조정
		(-P0 + 3.f * P1 - 3.f * P2 + P3) * T3);         // 3차 항 (S자 곡선 조정)
}

void USplineFollowerComponent::ResampleCatmullRom()
{
	const int32 N = PathPoints.Num();
	if (N < 3) return;
	
	// 루프, 비루프 경로를 모두 커버하는 인덱스 계산 람다 함수
	auto Idx = [&](int32 i) -> int32
	{
		// 루프이면 모듈러 연산으로 wrap-around, 아니면 끝점으로 클램핑
		// % 연산자는 음수 입력에 대해 음수를 반환할 수 있음. 이후 상황에서 크래시나지 않도록 (i % N) + N) % N
		return bClosedLoop ? ((i % N) + N) % N : FMath::Clamp(i, 0, N - 1);
	};
	
	// 보간된 경로 위치벡터 배열 = 차량이 지나갈 점들의 좌표 목록
	TArray<FVector> Out;
	
	Out.Reserve(N * 12); // 경험적 하드코딩 (CP 사이 간격이 500~600cm 정도, ResampleSpacing은 기본 50cm)
	
	// 루프 경로라면 끝점에서 wrap-around로 정확한 P0, P3값 가져옴
	// 비루프 경로라면 끝점에서만 약간 부자연스러움
			// [0]──[1]──[2]── ... ──[N-2]──[N-1]
			//  ↑                             ↑
			// P0가 없음                       P3가 없음
	for (int32 i = 0; i < N - 1; ++i)
	{
		const FVector& P0 = PathPoints[Idx(i - 1)];
		const FVector& P1 = PathPoints[i];
		const FVector& P2 = PathPoints[i + 1];
		const FVector& P3 = PathPoints[Idx(i + 2)];
		
		// P1~P2 구간의 길이를 ResampleSpacing(기본 50cm)으로 나눠 필요한 보간 스텝 수를 결정
		const int32 Steps = FMath::Max(
			1,
			FMath::CeilToInt32(FVector::Dist(P1, P2) / ResampleSpacing)
		);

		for (int32 s = 0; s < Steps; ++s)
		{
			FVector CatmullRom = EvalCatmullRom(P0, P1, P2, P3, (float)s / (float)Steps);
			Out.Add(CatmullRom);
		}
	}
	Out.Add(PathPoints.Last());
	
	// CP_A             CP_B                CP_C
	// ●────────────────●───────────────────●
  
	// ●──●────●──●─────●──●──●────●──●─────●   // GetPoints() 결과 (PathPoints)
												// 불규칙한 간격, 언리얼 엔진 내부 보간
	// ●─●─●─●─●─●─●─●─●─●─●─●─●─●─●─●─●─●─●─●  // ResampleCatmullRom 결과 (Out -> PathPoints 덮어씌우기)
												// 균일 간격, Catmull-Rom으로 부드럽게

	// 닫힌 루프이면 마지막 → 첫 포인트 구간도 보간
	if (bClosedLoop)
	{
		const FVector& P0 = PathPoints[N - 2]; const FVector& P1 = PathPoints[N - 1];
		const FVector& P2 = PathPoints[0];     const FVector& P3 = PathPoints[1];
		const int32 Steps = FMath::Max(1, FMath::CeilToInt32(FVector::Dist(P1, P2) / ResampleSpacing));
		for (int32 s = 1; s < Steps; ++s)
			Out.Add(EvalCatmullRom(P0, P1, P2, P3, (float)s / (float)Steps));
	}

	UE_LOG(LogAntarcitcKids, Log, TEXT("SplineFollower: Resampled %d -> %d pts"), N, Out.Num());
	PathPoints = MoveTemp(Out); // Out의 내부데이터를 PathPoints로 옮김, 복사하지 않고 소유권을 통째로 옮김
}


FVector USplineFollowerComponent::GetPointAhead(FVector& OutDir, float Distance) const
{
	// 현재 차량 위치에서 경로를 따라 Distance만큼 앞의 위치와 방향을 반환
	// 조향 계산 시 전방 주시 위치(LAPos)와 경로 방향(PathDir)을 구하는 핵심 함수

	if (OwnerPawn.IsValid() == false)
	{
		UE_LOG(LogPathFollowingComponent, Error, TEXT("Failed to cache OwnerPawn."));
		return FVector::ZeroVector;
	}
	
	const int32 Num = PathPoints.Num();
	const FVector Loc = OwnerPawn->GetActorLocation();

	// 현재 세그먼트(CurrentPointIndex → Nxt) 계산
	const int32 Nxt = bClosedLoop ? (CurrentPointIndex + 1) % Num
	                              : FMath::Min(CurrentPointIndex + 1, Num - 1);
	const FVector A = PathPoints[CurrentPointIndex], B = PathPoints[Nxt];
	const FVector AB = B - A;
	const float Len = AB.Size();
	
	
	// 차량을 현재 세그먼트(A→B)에 정사영하여 세그먼트 위의 비율 T를 구함
	// DotProduct(Loc-A, AB) / Len² : A에서 차량까지의 AB 방향 투영 비율
	const float T = (Len > KINDA_SMALL_NUMBER) // 0.0001f 정도로 아주 작은 수와 비교하여, 부동소수점 오차 줄이기
		? FMath::Clamp(FVector::DotProduct(Loc - A, AB) / (Len * Len), 0.f, 1.f) : 0.f;
	
	// 현재 세그먼트에서 차량 앞으로 남은 거리
	// T=0.3이면 30% 지점에 있으므로 남은 거리 = 70% * Len
	const float Remain = (1.f - T) * Len;


	// ── 목표 포인트가 현재 세그먼트 안에 있는 경우 ──────────────────────
	// Distance가 현재 세그먼트 안에서 해결되면 바로 반환
	if (Distance <= Remain && Len > KINDA_SMALL_NUMBER)
	{
		OutDir = AB.GetSafeNormal();
		return FMath::Lerp(A, B, FMath::Min(T + Distance / Len, 1.f));
	}

	// ── 목표 포인트가 다음 세그먼트에 있는 경우 ─────────────────────────
	// 현재 세그먼트에서 남은 거리를 소진하고, 다음 세그먼트들을 순회
	float Left = Distance - Remain;
	int32 Idx = Nxt;
	while (Left > 0.f)
	{
		const int32 Next = bClosedLoop ? (Idx + 1) % Num : Idx + 1;
		
		// 비루프 경로의 끝에 도달 → 마지막 포인트와 방향 반환 (마지막 세그먼트 방향을 그대로 유지)
		if (!bClosedLoop && Next >= Num)
		{
			OutDir = (PathPoints[Idx] - PathPoints[FMath::Max(0, Idx - 1)]).GetSafeNormal(); // OutDir 원본 수정
			return PathPoints[Idx];
		}
		
		const FVector Seg = PathPoints[Next] - PathPoints[Idx];
		const float SLen = Seg.Size();
		
		if (Left <= SLen && SLen > KINDA_SMALL_NUMBER)
		{
			OutDir = Seg.GetSafeNormal();
			return FMath::Lerp(PathPoints[Idx], PathPoints[Next], Left / SLen);
		}
		
		Left -= SLen;
		Idx = Next;
		if (Idx == CurrentPointIndex) break; // 루프를 한 바퀴 다 돌았으면 탈출
	}

	const int32 Prev = bClosedLoop ? ((Idx - 1 + Num) % Num) : FMath::Max(0, Idx - 1);
	OutDir = (PathPoints[Idx] - PathPoints[Prev]).GetSafeNormal();
	return PathPoints[Idx];
}


float USplineFollowerComponent::EstimateCurvature(float AheadOffset) const
{
	// 전방의 두 지점의 방향 벡터를 비교해 곡률(꺾임 정도)을 측정, 라디안으로 반환
	
	FVector D1, D2; // 방향을 받을 변수 (GetPointAhead()로 리턴되는 위치는 버림)
	
	GetPointAhead(D1, AheadOffset);
	GetPointAhead(D2, AheadOffset + CurvatureSampleSpan);
	
	// 두 방향 벡터의 내적으로 사이각(라디안) 계산
	return FMath::Acos(FMath::Clamp(FVector::DotProduct(D1, D2), -1.f, 1.f));
}


float USplineFollowerComponent::ComputeCurveSpeedLimit(float Curvature) const
{
	// '원심력 > 횡 마찰력'이면 미끄러지므로 곡률이 클수록 속도를 낮춰 안전한 주행을 보장
	
	// 곡률이 0에 가까우면 직선 → 최대 속도 허용
	if (Curvature <= KINDA_SMALL_NUMBER)
		return MaxSpeed;
	
	// 원심력(차를 바깥으로 미는 힘): F_c = mv² / R
	// 횡 마찰력(타이어가 버티는 힘) : F_f = μmg
	// v² ≤ μgR 이므로, V_max = sqrt(μ * g * R)
	
	// 두 샘플 포인트 사이 거리(호의 길이) = R × θ
	const float Radius = CurvatureSampleSpan / Curvature;
	return FMath::Clamp(
		FMath::Sqrt(LateralFriction * 980.f * Radius),
		MinSpeed, MaxSpeed
	);
}


// TickComponent() : 액터에 부착된 컴포넌트의 매 프레임 함수

// 목적 : 매 프레임 조향(3-way 블렌드) + 속도(곡률 기반 제한) 제어하며 경로를 추종
		// 현재 포인트 인덱스 전진 → 곡률 계산 → 조향 → 속도 제어 순으로 처리

// 조향 : 핸들을 돌리는 것
		// Steer = -1.0  →  최대 좌회전
		// Steer =  0.0  →  직진
		// Steer = +1.0  →  최대 우회전
		
// 3-way 블렌드
		// (1) PosDelta (Position Delta)  : 전방 주시 위치로의 각도 오차 -> "저기 목표 위치를 향해야 해"
		// (2) HdgDelta (Heading Delta)  : 경로 접선 방향으로의 각도 오차 -> "경로가 꺾이는 방향을 미리 틀어야 해"
		// (3) CrossErr (Crosstrack Error) : 중앙선으로부터의 수직 이탈 거리 -> "중앙선에서 너무 벗어났으니 보정해야 해"

void USplineFollowerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (OwnerPawn.IsValid() == false)
	{
		UE_LOG(LogPathFollowingComponent, Error, TEXT("Failed to cache OwnerPawn."));
		return;
	}
	if (PathPoints.Num() < 2)
	{
		UE_LOG(LogPathFollowingComponent, Error, TEXT("PathPoints has single point."));
		return;
	}

	const FVector Location = OwnerPawn->GetActorLocation();
	const float Velocity = OwnerPawn->GetVelocity().Size();
	
	// ── 현재 포인트 인덱스 전진 ──────────────────────────────────────────
	// 고속 이동 시 한 프레임에 여러 포인트를 넘길 수 있으므로 최대 스텝 수 제한
	// MaxSteps 없이 인덱스를 하나씩만 전진하면 이미 지나간 포인트를 못 넘긴 것으로 생각해서 조향이 뒤를 향하게 됨
	const int32 Max = bClosedLoop ? PathPoints.Num() : PathPoints.Num() - 2;
	const int32 MaxSteps = FMath::CeilToInt32(
		//Velocity / (60.f * FMath::Max(ResampleSpacing, 1.f))
		Velocity * DeltaTime / FMath::Max(ResampleSpacing, 1.f)
	) + 1;
	
	// Tick 동안 바뀐 실제 차량 위치에 맞게 CurrentPointIndex 갱신
	for (int32 S = 0; S < MaxSteps && CurrentPointIndex < Max; ++S)
	{
		const int32 Next = (CurrentPointIndex + 1) % PathPoints.Num();
		const FVector Seg = PathPoints[Next] - PathPoints[CurrentPointIndex];
		
		// 차량이 현재 세그먼트 끝을 지난 경우 : 내적 >= 세그먼트 길이의 제곱
		if (FVector::DotProduct(Location - PathPoints[CurrentPointIndex], Seg) >= Seg.SizeSquared())
			CurrentPointIndex = bClosedLoop ? Next : CurrentPointIndex + 1; // 방어 코드 : 어차피 Max 제한 때문에 비루프인 경우에도 Next 사용 가능
		else
			break; // 아직 현재 세그먼트 안에 있으면 더 이상 전진하지 않음
	}

	// 루프가 아닌 경로의 끝에 도달하면 정지
	if (!bClosedLoop && CurrentPointIndex >= PathPoints.Num() - 2)
	{
		OwnerPawn->DoThrottle(0.f); // 0.f : 가속 없음, 1.f : 최대 가속
		OwnerPawn->DoBrakeStart();
		return;
	}

	const float Yaw = OwnerPawn->GetActorRotation().Yaw;

	// ── 곡률 계산 ─────────────────────────────────────────────────────────
	// 커브 진입 전, 목표 속도에 도달하기 위해
	// 미리 감속을 시작하기 위한 거리(BrakePreviewDist) 만큼 앞부분을 미리 체크
	const float CurvHere  = EstimateCurvature(0.f);
	const float CurvAhead = EstimateCurvature(BrakePreviewDist); 

	// ── 조향 계산 ─────────────────────────────────────────────────────────
	// 커브가 심할수록 전방 주시 거리를 줄여 빠르게 반응
	// CurvHere=0(직선) → CurvScale=1.0, CurvHere 클수록 → CurvScale=0.5 에 수렴
	const float CurvScale = FMath::Lerp(
		1.f,
		0.5f,
		FMath::Clamp(CurvHere * 3.f, 0.f, 1.f)
	);
	
	// 속도가 빠를수록 더 멀리 볼 수 있게 세팅 후, CurvScale로 전방 주시 거리 축소
	const float LADist = (LookAheadBase + Velocity * LookAheadSpeedFactor) * CurvScale;

	FVector PathDir;
	const FVector LAPos = GetPointAhead(PathDir, LADist);
	// LAPos : 전방 LADist 거리의 경로 위치 
	// PathDir : LAPos에서의 경로 접선 방향 (빈 채로 전달되지만 함수 내부에서 참조로 채워짐)

	// (1) 전방 주시 위치로의 Yaw 오차 (위치 추종) : 차량 Yaw와 전방 주시 위치 방향의 최단 각도 차이
	const float PosDelta = FMath::FindDeltaAngleDegrees(
		Yaw,
		FMath::Atan2(
			(LAPos - Location).GetSafeNormal().Y,
			(LAPos - Location).GetSafeNormal().X
		) * (180.f / PI)
	);

	// (2) 경로 방향으로의 Yaw 오차 (커브 예측) : 차량 Yaw와 전방 주시 지점에서의 경로 접선 방향의 각도 차이
	const float HdgDelta = FMath::FindDeltaAngleDegrees(
		Yaw, FMath::Atan2(PathDir.Y, PathDir.X) * (180.f / PI));

	// (3) 중앙선으로부터의 수직 이탈 거리 (중심선 보정) : 현재 세그먼트 방향으로 투영 후 수직 성분만 추출
	const int32 Nxt = bClosedLoop ?
		(CurrentPointIndex + 1) % PathPoints.Num()
	     : FMath::Min(CurrentPointIndex + 1, PathPoints.Num() - 1);
	const FVector SegDir = (PathPoints[Nxt] - PathPoints[CurrentPointIndex]).GetSafeNormal();
	const FVector Offset = Location - PathPoints[CurrentPointIndex];
	// Offset에서 SegDir 방향 성분을 빼면 수직 성분(이탈 거리)만 남음
	const float CrossErr = FVector::DotProduct(
		Offset - SegDir * FVector::DotProduct(Offset, SegDir), // 수직 벡터
		FVector::CrossProduct(FVector::UpVector, SegDir) // 경로의 왼쪽 방향 단위벡터
	);

	// (1)~(3) 블렌딩 : heading (anticipation) + position (tracking) + crosstrack (centerline)
	// 커브가 심할수록 HeadingWeight를 줄여 위치 추종(PosDelta) 비중을 높임
	const float HdgW = FMath::Lerp(
		HeadingWeight,
		0.3f,
		FMath::Clamp(CurvHere * 3.f, 0.f, 1.f)
	);
	// 실제 급커브여도, 50cm 사이에서 5~10도 꺾임. CurvHere은 0.175 라디안 정도로 매우 작은 값
	// 경험적 조정 (CurvHere에 3배 처리) 필요. 3배도 미미할 수 있음
	
	const float YawCmd = PosDelta * (1.f - HdgW) + HdgDelta * HdgW;
	
	// YawCmd를 MaxYawDelta로 정규화하고 CrossErr 보정을 더해 -1~1로 클램핑
		// 차량이 왼쪽으로 이탈이면 → CrossErr > 0
		// Steer에 양수를 더해, 오른쪽으로 조향해야 함
	const float Steer = FMath::Clamp(
		YawCmd / MaxYawDelta - CrossErr * CrosstrackGain,
		-1.f, 1.f
	);
	OwnerPawn->DoSteering(Steer);

	// ── 속도 계산 ─────────────────────────────────────────────────────────
	// 현재와 전방 곡률 중 더 엄격한 제한 적용 → 커브 진입 전 미리 감속
	const float SpeedLimit = FMath::Min(
		ComputeCurveSpeedLimit(CurvHere),
		ComputeCurveSpeedLimit(CurvAhead)
	);

	// 감속은 빠르게(DecelRate), 가속은 천천히(AccelRate) 보간
	const float Rate = (SpeedLimit < SmoothedTargetSpeed) ? DecelRate : AccelRate;
	SmoothedTargetSpeed = FMath::FInterpTo(
		SmoothedTargetSpeed,
		SpeedLimit, DeltaTime,
		Rate
	);
	
	// 목표 속도와 현재 속도의 차이를 ThrottleGain으로 스케일하여 명령 생성
	// 양수 = 가속 필요, 음수 = 감속 필요
	const float Cmd = FMath::Clamp(
		(SmoothedTargetSpeed - Velocity) * ThrottleGain,
		-1.f, 1.f
	);
	
	
	// +-0.05 데드존 : 속도가 목표에 근접하면 스로틀/브레이크 모두 0 -> 미세한 진동(헌팅) 방지
	if (Cmd > 0.05f)
		OwnerPawn->DoThrottle(Cmd);
	else if (Cmd < -0.05f)
		OwnerPawn->DoBrake(-Cmd);
	else
	{
		OwnerPawn->DoThrottle(0.f);
		OwnerPawn->DoBrake(0.f);
	}
}
