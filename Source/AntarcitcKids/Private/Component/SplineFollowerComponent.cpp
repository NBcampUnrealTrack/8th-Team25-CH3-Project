// SplineFollowerComponent.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Component/SplineFollowerComponent.h"
#include "CityVehiclePawn.h"
#include "Road/SplineRoadActor.h"
#include "Components/SplineComponent.h"
#include "EngineUtils.h"


// 커스텀 로그 카테고리를 정의
DEFINE_LOG_CATEGORY_STATIC(LogSplineFollower, Log, All);

USplineFollowerComponent::USplineFollowerComponent()
	: EndDetectionThreshold(200.f)
	, VehicleMaxSpeedKMH(145.f)
	, RoadNormalSpeedKMH(50.f)
	, MinCurveSpeedKMH(15.f)
	, ThrottleGain(0.002f)
	, DecelRate(0.5f)
	, AccelRate(1.f)
	, bHasActiveSpeedLimit(false)
	, ActiveSpeedLimitKMH(0.f)
	, SmoothedTargetSpeed(0.f)
	, LookAheadBase(1500.f)
	, LookAheadSpeedFactor(0.3f)
	, MaxYawDelta(40.f)
	, HeadingWeight(0.7f)
	, CrosstrackGain(0.0015f)
	, LateralFriction(0.4f)
	, CurvatureSampleSpan(50.f)
	, BrakePreviewDist(3000.f)
	, FollowState(ESplineFollowState::Driving)
	, ProgressDistance(0.f)
	, SplineTotalLength(0.f)
	, bSplineIsClosedLoop(false)
	
{
	PrimaryComponentTick.bCanEverTick = true;
	
	// BuildPath() 완료 후, 경로가 준비되면 Tick 활성화
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USplineFollowerComponent::EmergencyStop()
{
	if (FollowState != ESplineFollowState::EmergencyStopping)
	{
		FollowState = ESplineFollowState::EmergencyStopping;
		UE_LOG(LogSplineFollower, Log, TEXT("EmergencyStop: Transition to EmergencyStopping state."));
		OnEmergencyStop.Broadcast();
	}
}

void USplineFollowerComponent::SmoothStop()
{
	if (!IsAnyStopActive())
	{
		FollowState = ESplineFollowState::SmoothStopping;
		UE_LOG(LogSplineFollower, Log, TEXT("SmoothStop: Transition to SmoothStopping state."));
	}
}

void USplineFollowerComponent::ResumeDriving()
{
	// 루프가 아닌데, 경로 끝에 다다른 경우, 무시
	if (IsAnyStopActive())
	{
		if (bSplineIsClosedLoop || ProgressDistance < SplineTotalLength - KINDA_SMALL_NUMBER)
		{
			FollowState = ESplineFollowState::Driving;
			OwnerPawn->ResumeMovement();
			UE_LOG(LogSplineFollower, Log, TEXT("ResumeDriving: Transition to Driving state."));
		}
	}
}

void USplineFollowerComponent::SetNormalSpeed(float SpeedKMH)
{
	RoadNormalSpeedKMH = SpeedKMH;
	UE_LOG(LogSplineFollower, Log, TEXT("SetNormalSpeed: %.0f km/h (%.0f cm/s)"), SpeedKMH, KMHToCMS(SpeedKMH));
}


void USplineFollowerComponent::SetSpeedLimit(float SpeedLimitKMH)
{
	ActiveSpeedLimitKMH = SpeedLimitKMH;
	bHasActiveSpeedLimit = true;
	
	UE_LOG(LogSplineFollower, Log,
		TEXT("SetSpeedLimit: %.0f km/h (%.0f cm/s)"),
		SpeedLimitKMH, KMHToCMS(SpeedLimitKMH));
}

void USplineFollowerComponent::ClearSpeedLimit()
{
	if (!bHasActiveSpeedLimit) return;
	
	bHasActiveSpeedLimit = false;
	UE_LOG(LogSplineFollower, Log, TEXT("ClearSpeedLimit: Speed limit removed."));
}

void USplineFollowerComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerPawn = Cast<ACityVehiclePawn>(GetOwner());
	if (!OwnerPawn.IsValid())
	{
		UE_LOG(LogSplineFollower, Error, 
			TEXT("BeginPlay: Owner is not ACityVehiclePawn."));
		return;
	}
	
	BuildPath();
}

void USplineFollowerComponent::BuildPath()
{
	// 재호출 대비 데이터 초기화
	CurrentSpline.Reset();
	SplineTotalLength = 0.f;
	bSplineIsClosedLoop = false;
	ProgressDistance = 0.f;
	SmoothedTargetSpeed = 0.f;
	SetComponentTickEnabled(false);
	
	
	// ──────────────────────────────────────────────────────────────
	// 추종할 도로 결정
	// 1) StartingRoad가 지정되어 있으면 그것을 사용
	// 2) 미지정이면 가장 가까운 도로 자동 탐색
	// ──────────────────────────────────────────────────────────────
    
	ASplineRoadActor* TargetRoad = StartingRoad;
	
	if (!TargetRoad)
	{
		TargetRoad = FindNearestRoad();
		if (!TargetRoad)
		{
			UE_LOG(LogSplineFollower, Warning,
				TEXT("BuildPath: No ASplineRoadActor found."));
			return;
		}
		UE_LOG(LogSplineFollower, Log,
			TEXT("BuildPath: StartingRoad unset. Using nearest road: %s"),
			*TargetRoad->GetName());
	}
	
	// ──────────────────────────────────────────────────────────────
	// 스플라인 컴포넌트 캐시 + 메타 정보 추출
	// ──────────────────────────────────────────────────────────────
    
	USplineComponent* Spline = TargetRoad->GetSplineComponent();
	if (!Spline || Spline->GetNumberOfSplinePoints() < 2)
	{
		UE_LOG(LogSplineFollower, Warning,
			TEXT("BuildPath: Road '%s' has no valid spline."),
			*TargetRoad->GetName());
		return;
	}
	
	CurrentSpline = Spline;
	SplineTotalLength = Spline->GetSplineLength();
	bSplineIsClosedLoop = Spline->IsClosedLoop();
	
	// ──────────────────────────────────────────────────────────────
	// 차량의 현재 위치 → 스플라인 상의 진행 거리로 변환
	// ──────────────────────────────────────────────────────────────
    
	InitializeProgress();
	
	SmoothedTargetSpeed = KMHToCMS(RoadNormalSpeedKMH);
	FollowState = ESplineFollowState::Driving;
	SetComponentTickEnabled(true);
	
	UE_LOG(LogSplineFollower, Log,
		TEXT("BuildPath: Road='%s', Length=%.0fcm, Loop=%s, StartDist=%.0fcm, RoadNormalSpeedKMH=%.0fkm/h"),
		*TargetRoad->GetName(), SplineTotalLength,
		bSplineIsClosedLoop ? TEXT("Y") : TEXT("N"), 
		ProgressDistance, RoadNormalSpeedKMH);
}

ASplineRoadActor* USplineFollowerComponent::FindNearestRoad() const
{
	// 월드에 배치된 모든 ASplineRoadActor를 순회하며 차량 위치와의 거리 비교
	// O(N) 순회, BeginPlay에서 1회 호출됨
	
	if (!OwnerPawn.IsValid()) return nullptr;
	
	const FVector VehicleLoc = OwnerPawn->GetActorLocation();
	
	ASplineRoadActor* BestRoad = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();
	
	for (TActorIterator<ASplineRoadActor> It(GetWorld()) ; It ; ++It )
	{
		ASplineRoadActor* Road = *It;
		USplineComponent* Spline = Road ? Road->GetSplineComponent() : nullptr;
		if (!Spline || Spline->GetNumberOfSplinePoints() < 2) continue;
		
		// FindLocationClosestToWorldLocation: 차량 위치에 가장 가까운 스플라인 상 점을 반환
		const FVector ClosestOnSpline = Spline->FindLocationClosestToWorldLocation(
			VehicleLoc, ESplineCoordinateSpace::World);
		
		const float DistSq = FVector::DistSquared(ClosestOnSpline, VehicleLoc);
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq; 
			BestRoad = Road;
		}
	}
	
	return BestRoad;
}

void USplineFollowerComponent::InitializeProgress()
{
	// 차량의 현재 월드 위치를 스플라인 상의 진행 거리(cm)로 변환
	// FindInputKeyClosestToWorldLocation: 차량과 가장 가까운 스플라인 파라미터(InputKey) 반환
		// InputKey는 0.0 = 첫 SplinePoint, 1.0 = 두 번째 SplinePoint, ... 식의 비율값
	// GetDistanceAlongSplineAtSplineInputKey: 그 InputKey를 누적 거리(cm)로 환산
	
	if (!OwnerPawn.IsValid() || !CurrentSpline.IsValid())
	{
		ProgressDistance = 0.f;
		return;
	}
	
	const FVector VehicleLoc = OwnerPawn->GetActorLocation();
	USplineComponent* Spline = CurrentSpline.Get(); // WeakPtr → raw 포인터 변환 (TWeakObjectPtr는 직접 메서드 호출이 안 됨)
	
	const float InputKey = Spline->FindInputKeyClosestToWorldLocation(VehicleLoc);
	ProgressDistance = Spline->GetDistanceAlongSplineAtSplineInputKey(InputKey);
}




// TickComponent() : 매 프레임 조향(3-way 블렌드) + 속도(곡률 기반 제한) 제어하며 경로를 추종
// 흐름:
//   1) 외부 명령 상태(FollowState)에 따른 분기 처리
//   2) 차량의 스플라인 진행 거리 갱신 (UpdateProgress)
//   3) 끝 도달 시 정차 처리
//   4) 조향/스로틀브레이크 산출 후 Pawn에 전달

void USplineFollowerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (! OwnerPawn.IsValid() || !CurrentSpline.IsValid()) return;
	
	// ── 외부 명령 상태 처리 ──────────────────────────────────────
	if (FollowState == ESplineFollowState::EmergencyStopping)
	{
		OwnerPawn->DoFullStop();
		return;
	}
	
	// ── 진행 거리 갱신 및 끝 도달 판정 ────────────────────────────
	const bool bReachedEnd = UpdateProgress(DeltaTime);
	
	if (bReachedEnd)
	{
		//OwnerPawn->DoFullStop();
		
		// NextRoads가 있으면 여기서 다음 도로로 전환 (현재는 정차)
		//FollowState = ESplineFollowState::EmergencyStopping;
		//UE_LOG(LogSplineFollower, Log, TEXT("Tick: Reached end of road. Stopping."));
		//return;
		BuildPath();
	}
	
	// ── 제어값 산출 및 적용 ──────────────────────────────────────
	const float CurvHere = EstimateCurvatureAt(0.f);
	const float CurvAhead = EstimateCurvatureAt(BrakePreviewDist);
	
	float Steering = ComputeSteeringInput(CurvHere);
	float ThrottleBrakeCmd = ComputeThrottleBrakeCommand(DeltaTime, CurvHere, CurvAhead);
	
	ApplyControls(Steering,  ThrottleBrakeCmd);
}



bool USplineFollowerComponent::UpdateProgress(float DeltaTime)
{
	// 차량의 실제 월드 위치를 기반으로 스플라인 상의 진행 거리(ProgressDistance) 갱신
	// 비루프 도로의 끝에 도달했으면 true 반환
	// 루프 도로는 SplineTotalLength를 넘어가면 시작점으로 wrap around 처리, false 반환
	
	USplineComponent* Spline = CurrentSpline.Get();
	const FVector VehicleLoc = OwnerPawn->GetActorLocation();
	
	const float InputKey = Spline->FindInputKeyClosestToWorldLocation(VehicleLoc);
	const float NewDistance = Spline->GetDistanceAlongSplineAtSplineInputKey(InputKey);
	
	// ── 닫힌 루프 도로 처리 ──────────────────────────────────────
	if (bSplineIsClosedLoop)
	{
		ProgressDistance = FMath::Fmod(NewDistance, SplineTotalLength);
		if (ProgressDistance < 0) ProgressDistance += SplineTotalLength;
		return false;
	}
	
	// ── 열린 도로 처리 ──────────────────────────────────────────
	ProgressDistance = NewDistance;
	const float RemainingDistance  = SplineTotalLength - ProgressDistance;
	
	return RemainingDistance <= EndDetectionThreshold;
}

float USplineFollowerComponent::ComputeSteeringInput(float CurvHere) const
{
	// 3-way 블렌드
	// (1) PosDelta (Position Delta)  : 전방 주시 위치로의 각도 오차 -> "저기 목표 위치를 향해야 해"
	// (2) HdgDelta (Heading Delta)  : 경로 접선 방향으로의 각도 오차 -> "경로가 꺾이는 방향을 미리 틀어야 해"
	// (3) CrossErr (Crosstrack Error) : 중앙선으로부터의 수직 이탈 거리 -> "중앙선에서 너무 벗어났으니 보정해야 해"

	USplineComponent* Spline = CurrentSpline.Get();
	const FVector Location = OwnerPawn->GetActorLocation();
	const float Velocity = OwnerPawn->GetForwardSpeed(); // 전진 속도만 얻기 위해 ACityVehiclePawn 쪽에 함수 만들어둠
	const float Yaw = OwnerPawn->GetActorRotation().Yaw;
	
	// ── 전방 주시 거리 계산 ──────────────────────────────────────
	// CurvScale : CurvHere = 0(직선)이면 1.0, 커질수록 → 0.5 에 수렴
	// 속도가 빠를수록 더 멀리 볼 수 있게 세팅 후, CurvScale로 전방 주시 거리 축소
	const float CurvScale = FMath::Lerp(
		1.f,
		0.5f,
		FMath::Clamp(CurvHere * 3.f, 0.f, 1.f)
	);
	
	const float LADist = (LookAheadBase + Velocity * LookAheadSpeedFactor) * CurvScale;

	
	// ── 전방 주시 위치 및 경로 방향 산출 ────────────────────────
	
	const float LookAheadDistance = ProgressDistance + LADist;
	float ClampedLA;
	
	// 루프 경로면 wrap, 비루프면 끝지점까지만
	if (bSplineIsClosedLoop)
	{
		ClampedLA = FMath::Fmod(LookAheadDistance, SplineTotalLength);
		if (ClampedLA < 0.f) ClampedLA += SplineTotalLength;
	}
	else
	{
		ClampedLA = FMath::Min(LookAheadDistance, SplineTotalLength);
	}
	
	const FVector LAPos = Spline->GetLocationAtDistanceAlongSpline(ClampedLA, ESplineCoordinateSpace::World);
	const FVector PathDir = Spline->GetDirectionAtDistanceAlongSpline(ClampedLA, ESplineCoordinateSpace::World);
	
	
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
		Yaw, 
		FMath::Atan2(
			PathDir.Y, 
			PathDir.X
		) * (180.f / PI));

	// (3) 중앙선으로부터의 수직 이탈 거리 (중심선 보정)
	const FVector NearestOnPath  = Spline->GetLocationAtDistanceAlongSpline(ProgressDistance, ESplineCoordinateSpace::World);
	const FVector PathTangent = Spline->GetDirectionAtDistanceAlongSpline(ProgressDistance, ESplineCoordinateSpace::World).GetSafeNormal(); // 단위벡터 반환하지만 방어코드
	
	// 도로 표면의 법선벡터, 오른쪽 방향 단위벡터 (경사로가 있는 환경을 고려)
	const FVector ProjectedUp = FVector::UpVector - PathTangent * FVector::DotProduct(FVector::UpVector, PathTangent);
	const FVector RoadNormal = ProjectedUp.GetSafeNormal();
	const FVector PathRight = FVector::CrossProduct(RoadNormal, PathTangent);
	
	// 수직 이탈 성분 : Offset에서 PathTangent(단위벡터) 방향 성분 제거
	const FVector Offset = Location - NearestOnPath;
	const FVector OffsetPerp = Offset - PathTangent * FVector::DotProduct(Offset, PathTangent);
	
	// 양수 → 도로 오른쪽으로 이탈, 음수 → 왼쪽으로 이탈
	const float CrossErr = FVector::DotProduct(OffsetPerp, PathRight);
	
	// ── (1)~(3) 가중치 블렌딩 ───────────────────────────────────
	//  블렌딩 : heading (anticipation) + position (tracking) + crosstrack (centerline)
	// 커브가 심할수록 HeadingWeight를 줄여 위치 추종(PosDelta) 비중을 높임
	const float HdgW = FMath::Lerp(
		HeadingWeight,
		0.3f,
		FMath::Clamp(CurvHere * 3.f, 0.f, 1.f)
	);
	const float YawCmd = PosDelta * (1.f - HdgW) + HdgDelta * HdgW;
	
	// ── 최종 조향값 정규화 ──────────────────────────────────────
	// YawCmd를 MaxYawDelta로 정규화하고 CrossErr 보정을 더해 -1~1로 클램핑
	// 차량이 오른쪽으로 이탈(CrossErr > 0)이면, Steer에서 값을 빼서, 왼쪽으로 조향
	const float Steering = FMath::Clamp(
		YawCmd / MaxYawDelta - CrossErr * CrosstrackGain,
		-1.f, 1.f
	);
	
	return Steering;
}

float USplineFollowerComponent::ComputeThrottleBrakeCommand(float DeltaTime, float CurvHere, float CurvAhead)
{
	// 곡률 기반 안전 속도와 현재 속도 차이로 스로틀/브레이크 명령(-1~1) 산출
	// 양수 → 스로틀, 음수 → 브레이크, 0 부근 → 둘 다 0 (관성 주행)
	
	const float Velocity = OwnerPawn->GetForwardSpeed();
	
	// ── 곡률 기반 속도 계산 ──────────────────────────────────────
	// 현재와 전방 곡률 중 더 엄격한 제한 적용 → 커브 진입 전 미리 감속
	
	const float CurveSpeedLimitKMH = FMath::Min(
		ComputeCurveSpeedLimitKMH(CurvHere),
		ComputeCurveSpeedLimitKMH(CurvAhead)
	);
	
	// ── 기본 목표 속도 ──────────────────────────────────────────
	// 도로 기본 속도 vs 곡률 안전 속도 중 작은 값
	float SpeedLimitKMH = FMath::Min(RoadNormalSpeedKMH, CurveSpeedLimitKMH);
	
	// ── 외부(AI) 시나리오 제한 ──────────────────────────────────
    // 스쿨존, 공사구간 등 추가 제한 적용
	if (bHasActiveSpeedLimit)
	{
		SpeedLimitKMH = FMath::Min(SpeedLimitKMH, ActiveSpeedLimitKMH);
	}
	
	if (FollowState == ESplineFollowState::SmoothStopping)
	{
		SpeedLimitKMH = 0.f;
	}

	// ── 목표 속도 보간 ──────────────────────────────────────────
	// 감속은 빠르게(DecelRate), 가속은 천천히(AccelRate) 보간
	
	const float SpeedLimitCMS = KMHToCMS(SpeedLimitKMH);
	
	const float Rate = (SpeedLimitCMS < SmoothedTargetSpeed) ? DecelRate : AccelRate;
	SmoothedTargetSpeed = FMath::FInterpTo(
		SmoothedTargetSpeed, SpeedLimitCMS, DeltaTime,Rate
	);
	
	// ── 스로틀/브레이크 명령 ─────────────────────────────────────
	// (목표 - 현재) × 게인 → 양수면 가속, 음수면 감속
	const float Cmd = FMath::Clamp(
		(SmoothedTargetSpeed - Velocity) * ThrottleGain,
		-1.f, 1.f
	);
	
	return Cmd;
}

void USplineFollowerComponent::ApplyControls(float Steering, float ThrottleBrakeCmd)
{
	
	OwnerPawn->DoSteering(Steering);
	
	// +-0.05 데드존 : 속도가 목표에 근접하면 스로틀/브레이크 모두 0 -> 미세한 진동(헌팅) 방지
	if (ThrottleBrakeCmd > 0.05f)
	{
		OwnerPawn->DoThrottle(ThrottleBrakeCmd);
	}
	else if (ThrottleBrakeCmd < -0.05f)
	{
		OwnerPawn->DoBrake(-ThrottleBrakeCmd); // 음수니까 양수로 넣어주기
	}
	else
	{
		OwnerPawn->DoThrottle(0.f);
		OwnerPawn->DoBrake(0.f);
	}
}


float USplineFollowerComponent::EstimateCurvatureAt(float OffsetAhead) const
{
	// 전방의 두 지점의 방향 벡터를 비교해 곡률(꺾임 정도)을 측정, 라디안으로 반환
		// 직선 → 사이각 0 (곡률 없음)
		// 급커브 → 사이각 큼 (곡률 큼)
	
	if (!CurrentSpline.IsValid()) return 0.f;
	
	USplineComponent* Spline = CurrentSpline.Get();
	
	const float Dist1 = ProgressDistance + OffsetAhead;
	const float Dist2 = Dist1 + CurvatureSampleSpan;
	
	auto ClampDist = [this](float Dist) -> float
	{
		if (bSplineIsClosedLoop)
		{
			float Wrapped = FMath::Fmod(Dist, SplineTotalLength);
			if (Wrapped < 0.f) Wrapped += SplineTotalLength;		// 향후 "뒤쪽" 곡률 측정이 필요해질 경우를 대비
			return Wrapped;
		}
		return FMath::Clamp(Dist, 0.f, SplineTotalLength);
	};
	
	const FVector Dir1 = Spline->GetDirectionAtDistanceAlongSpline(
		ClampDist(Dist1), ESplineCoordinateSpace::World);
	const FVector Dir2 = Spline->GetDirectionAtDistanceAlongSpline(
		ClampDist(Dist2), ESplineCoordinateSpace::World);
	
	// 두 방향 벡터의 내적으로 사이각(라디안) 계산
	const float Dot = FMath::Clamp(FVector::DotProduct(Dir1, Dir2), -1.f, 1.f);
	return FMath::Acos(Dot);
}


float USplineFollowerComponent::ComputeCurveSpeedLimitKMH(float Curvature) const
{
	// 곡률(라디안)을 입력받아 원심력-횡마찰 균형에 따른 안전 최대 속도 산출
		// 원심력(차를 바깥으로 미는 힘): F_c = mv² / R
		// 횡 마찰력(타이어가 버티는 힘) : F_f = μmg
		// 미끄러지지 않으려면 F_c <= F_f
		// 즉, V_max = sqrt(μ * g * R)
	
	// 곡률이 0에 가까우면 직선 → 차량 최대 속도까지 허용
	if (Curvature <= KINDA_SMALL_NUMBER) return VehicleMaxSpeedKMH;
	
	// 두 샘플 포인트 사이 거리(호의 길이) = R × θ
	const float Radius = CurvatureSampleSpan / Curvature;
	const float MaxSafeSpeedCMS = FMath::Sqrt(LateralFriction * 980.f * Radius);	// cm/s 단위 기반의 공식
	const float MaxSafeSpeedKMH = CMSToKMH(MaxSafeSpeedCMS);
	
	return FMath::Clamp(MaxSafeSpeedKMH, MinCurveSpeedKMH, VehicleMaxSpeedKMH);
}
