// LidarSceneComponent.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Sensor/LidarSceneComponent.h"
#include "Sensor/LidarBevRenderer.h"
#include "Engine/World.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/Paths.h"
#include "TimerManager.h"
#include "Sensor/LidarNiagaraComponent.h"
#include "Manager/SensorSubSystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogLidarSensor, Log, All);

ULidarSceneComponent::ULidarSceneComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	//시작 시 즉시 tick이 발동되지 않도록 한다.
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	Tag = TEXT("Mission");
}

void ULidarSceneComponent::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp,Warning,TEXT("ULidarSceneComponent Activate"));
	ApplyPreset(Preset);
	
	InitializeSensor();
	StartScan();
	
	/*GetWorld()->GetTimerManager().SetTimer(
		MakeBoundingBox,
		this,
		&ULidarSceneComponent::BroadCastActor,
		0.1f,
		true);*/
	
}

void ULidarSceneComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopScanTimer();
	Super::EndPlay(EndPlayReason);
}

void ULidarSceneComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bHasPendingTraces && GFrameCounter > FireFrameNumber)
	{

		CollectAsyncResults();
		SetComponentTickEnabled(false);
	}
}

//Preset 설정이 바뀌었을 경우, 다시 거리를 계산하여 주는 함수
//에디터 값이 바뀌는 순간, PostEditChangeProperty 자동 발동
#if WITH_EDITOR
void ULidarSceneComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropName = PropertyChangedEvent.GetPropertyName();
	if (PropName == GET_MEMBER_NAME_CHECKED(ULidarSceneComponent, Preset))
		ApplyPreset(Preset);
	bDirectionsDirty = true;
}
#endif

//센서 초기화
void ULidarSceneComponent::InitializeSensor()
{
	BevConfig.ViewRange = Config.MaxRange;
	//Bev 라이더 센서 렌더링을 위한 오브젝트
	BevRenderer = NewObject<ULidarBevRenderer>(this, TEXT("BevRenderer"));
	BevRenderer->Initialize(BevConfig);
	
	
	/*LidarSensorRenderer = NewObject<ULidarNiagaraComponent>(this, TEXT("LidarRenderer"));
	LidarSensorRenderer->SetupAttachment(this);
	LidarSensorRenderer->RegisterComponent();*/
	
	//센서 서브시스템 제어 추가
	USensorSubSystem* SensorSubSystem =  GetWorld()->GetSubsystem<USensorSubSystem>();
	
	if (SensorSubSystem)
	{
		SensorSubSystem->OnSensorPresetChanged.AddUObject(this, &ULidarSceneComponent::ApplyPreset);
		SensorSubSystem->OnSetSensorHz.AddUObject(this,&ULidarSceneComponent::SetScanRate);
	}
	

	//매 틱마다 메모리에 동적 할당 되지 않도록 최적화
	const int32 TotalPts = Config.GetTotalPoints();
	PendingHandles.Reserve(TotalPts); //비동기 레이트레이스 핸들 목록
	PendingWorldDirs.Reserve(TotalPts); //각 레이의 월드 공간 방향 벡터
	ScanPoints.Reserve(TotalPts); // 레이가 충돌한 3D 좌표 결과
	ScanIntensities.Reserve(TotalPts); // 반사 강도
	LastPointCloud.Points.Reserve(TotalPts); //마지막 프레임에 완성된 포인터 클라우드
	LastPointCloud.Intensities.Reserve(TotalPts);
	
	//라이다 센서는 특정 방향을 계산하여 정밀하게 방사.
	//이 과정은 대게 리소스가 많이 소모되므로, 미리 계산하는데, 그걸 계산해야 하는지 여부에 관한 플레그
	bDirectionsDirty = true;
	bSensorEnabled = false;

	UE_LOG(LogLidarSensor, Log,
		TEXT("LidarSensor initialized: %d ch x %d pts @ %.0f Hz, range %.0f m  [AsyncTrace]"),
		Config.NumChannels, Config.PointsPerChannel,
		Config.RotationRate, Config.MaxRange / 100.0f
	);
}

//센서의 방향 캐시를 저장
void ULidarSceneComponent::RebuildDirectionCache()
{
	//세로 줄 (높이 분단)
	const int32 NumCh    = Config.NumChannels;
	//가로 줄 (360도를 나누어서)
	const int32 NumPts   = Config.PointsPerChannel;
	const float VertLow  = Config.VerticalFOVLower;
	const float VertRng  = Config.VerticalFOVUpper - VertLow;
	const float HorizFOV = Config.HorizontalFOV;
	
	//캐시에 저장
	CachedLocalDirections.SetNum(NumCh * NumPts, EAllowShrinking::No);

	for (int32 Ch = 0; Ch < NumCh; ++Ch)
	{
		const float VertDeg  = (NumCh > 1) ? VertLow + VertRng * (float(Ch) / (NumCh - 1)) : 0.f;
		const float CosVert  = FMath::Cos(FMath::DegreesToRadians(VertDeg));
		const float SinVert  = FMath::Sin(FMath::DegreesToRadians(VertDeg));

		for (int32 Pt = 0; Pt < NumPts; ++Pt)
		{
			const float HorizRad = FMath::DegreesToRadians((float(Pt) / NumPts) * HorizFOV);
			CachedLocalDirections[Ch * NumPts + Pt] = FVector(
				CosVert * FMath::Cos(HorizRad),
				CosVert * FMath::Sin(HorizRad),
				SinVert
			);
		}
	}

	bDirectionsDirty = false;
}

void ULidarSceneComponent::DetectActor(TArray<AActor*> DetectedTagActors)
{
	/*UE_LOG(LogTemp, Error, TEXT("DetectActor 작동!"));*/
	ImpactActorReady.Broadcast(DetectedTagActors,Tag);
	
		
}

void ULidarSceneComponent::BroadCastActor()
{
	ImpactActorReady.Broadcast(PendingDetectedActor,Tag);
}


//스캔 타이머 시작
void ULidarSceneComponent::StartScanTimer()
{
	if (GetWorld() == nullptr) return;
	
	const float Interval = 1.0f / FMath::Max(Config.RotationRate, 1.0f);
	GetWorld()->GetTimerManager().SetTimer(
		ScanTimerHandle,
		this,
		&ULidarSceneComponent::OnScanTimer,
		Interval,
		true);
}

void ULidarSceneComponent::StopScanTimer()
{
	if (GetWorld())
		GetWorld()->GetTimerManager().ClearTimer(ScanTimerHandle);
	
	bHasPendingTraces = false;
	SetComponentTickEnabled(false);
}

//센서가 시행될 때
void ULidarSceneComponent::OnScanTimer()
{
	//센서 준비 여부 확인
	if (!bSensorEnabled)
		return;
	//방향 계산 여부 확인
	if (bDirectionsDirty)
		RebuildDirectionCache();

	FireAsyncTraces();

	++FrameCount;
}

void ULidarSceneComponent::FireAsyncTraces()
{
	UWorld* World = GetWorld();
	if (!World || CachedLocalDirections.IsEmpty())
		return;

	//지난 트레이스의 결과를 모두 지운다
	PendingHandles.Reset();
	PendingWorldDirs.Reset();
	
	//센서의 현재 위치와 회전값을 가져온다.
	const FTransform SensorTransform = GetComponentTransform();
	const FVector    SensorLoc       = SensorTransform.GetLocation();
	const FQuat      SensorQuat      = SensorTransform.GetRotation();
	PendingTransform = SensorTransform;

	//충돌 쿼리 설정
	FCollisionQueryParams Params(SCENE_QUERY_STAT(LidarAsyncTrace), false);
	Params.AddIgnoredActor(GetOwner());
	Params.bReturnPhysicalMaterial = false;

	const float MaxRange = Config.MaxRange;

	//비동기 레이 트레이스를 발사한다.
	for (const FVector& LocalDir : CachedLocalDirections)
	{
		const FVector WorldDir = SensorQuat.RotateVector(LocalDir);
		const FVector End      = SensorLoc + WorldDir * MaxRange;

		FTraceHandle Handle = World->AsyncLineTraceByChannel(
			EAsyncTraceType::Single,
			SensorLoc, End,
			ECC_Visibility,
			Params
		);
		//Handle을 통해서 나중에 결과를 수거한다.
		PendingHandles.Add(Handle);
		PendingWorldDirs.Add(WorldDir);
	}
	
	//수거 안된 결과 존재함.
	FireFrameNumber   = GFrameCounter;
	bHasPendingTraces = true;

	SetComponentTickEnabled(true);
}

//비동기 레이 트레이스의 결과를 모은다.
void ULidarSceneComponent::CollectAsyncResults()
{
	/*UE_LOG(LogTemp, Warning, TEXT("CollectAsyncResults 발동"));*/
	UWorld* World = GetWorld();
	if (!World) return;

	ScanPoints.Reset();
	ScanIntensities.Reset();
	DetectedActors.Reset();
	PendingDetectedActor.Empty();

	const float MaxRange = Config.MaxRange;
	const float MinRange = Config.MinRange;
	const float NoiseStd = Config.NoiseStdDev;
	
	TArray<AActor*> DetectedTagActors;
	AActor* DetectedTagActor = nullptr;
	
	for (int32 i = 0; i < PendingHandles.Num(); ++i)
	{
		FTraceDatum Data;
		//비동기 트레이스의 결과가 준비되었는지 묻는 함수
		if (!World->QueryTraceData(PendingHandles[i], Data)) continue;
		if (Data.OutHits.IsEmpty()) continue;

		const FHitResult& Hit = Data.OutHits[0];
		if (!Hit.bBlockingHit || Hit.Distance < MinRange) continue;
		
		
		if (Hit.GetActor()->ActorHasTag(Tag))
		{
			/*UE_LOG(LogTemp, Warning, TEXT("특정 액터 탐지됨"));*/
			DetectedTagActors.Add(Hit.GetActor());
			DetectedTagActor = Hit.GetActor();
		}
		
		
		FVector HitPoint = Hit.ImpactPoint;
		//라이더 센서에 내재되어 있는 오차 추가 적용
		if (NoiseStd > 0.f && PendingWorldDirs.IsValidIndex(i))
		{
			HitPoint += PendingWorldDirs[i] * FMath::RandRange(-NoiseStd, NoiseStd);
		}
		
		//거리가 가까울 수록 1에 가깝게, 멀수록 0으로
		ScanPoints.Add(HitPoint);
		ScanIntensities.Add(FMath::Clamp(1.f - (Hit.Distance / MaxRange), 0.f, 1.f));
	}
	
	//일일히 복사하기 보다 MoveTemp를 써서 이동(포인터만 이동 및 원본 삭제)
	LastPointCloud.Points      = MoveTemp(ScanPoints);
	LastPointCloud.Intensities = MoveTemp(ScanIntensities);
	LastPointCloud.PointCount  = LastPointCloud.Points.Num();
	LastPointCloud.FrameNumber = FrameCount;

	//버퍼 재준비
	ScanPoints.Reserve(Config.GetTotalPoints());
	ScanIntensities.Reserve(Config.GetTotalPoints());

	//렌더 및 저장
	if (BevRenderer)
		BevRenderer->RenderPointCloud(LastPointCloud, PendingTransform);
	
	/*if (LidarSensorRenderer)
		LidarSensorRenderer->RenderPointCloudNiagara(LastPointCloud);*/
	
	

	if (OnPointCloudReady.IsBound())
	{
		/*UE_LOG(LogTemp, Warning, TEXT("LastPointCloudReady!"));*/
		OnPointCloudReady.Broadcast(LastPointCloud);
	}
	else
	{
		
		UE_LOG(LogTemp, Error, TEXT("[Liadar] %p 번지 센서에 바인딩이 존재하지 않습니다!!"), this);
	}


	
	
	/*PendingDetectedActor = DetectedTagActors;*/
	/*ImpactActorReady.Broadcast(DetectedTagActors,Tag);*/
	/*ImpactOneActorReady.Broadcast(DetectedTagActor,Tag);*/

	

		
	
	if (bIsDataSaving && LastPointCloud.PointCount > 0)
		SavePointCloudData();

	bHasPendingTraces = false;

	UE_LOG(LogLidarSensor, Verbose,
		TEXT("LidarSensor frame %lld: %d points"), FrameCount, LastPointCloud.PointCount);
}

void ULidarSceneComponent::StartScan()
{
	bSensorEnabled = true;
	StartScanTimer();
}

void ULidarSceneComponent::StopScan()
{
	bSensorEnabled = false;
	StopScanTimer();
}

void ULidarSceneComponent::SetScanRate(float Hz)
{
	Config.RotationRate = FMath::Clamp(Hz, 1.0f, 30.0f);
	StopScanTimer();
	if (bSensorEnabled)
		StartScanTimer();
}

void ULidarSceneComponent::RefreshSettings()
{
	bDirectionsDirty = true;
	BevConfig.ViewRange = Config.MaxRange;
	
	if (BevRenderer)
		BevRenderer->UpdateConfig(BevConfig);
	
	StopScanTimer();
	
	if (bSensorEnabled)
		StartScanTimer();
}

UTexture2D* ULidarSceneComponent::GetBevRenderTarget() const
{
	return BevRenderer ? BevRenderer->GetRenderTarget() : nullptr;
}

void ULidarSceneComponent::ApplyPreset(ELidarSensorPreset NewPreset)
{
	Preset = NewPreset;
	bDirectionsDirty = true;

	switch (NewPreset)
	{
	case ELidarSensorPreset::VelodyneVLP16:
		Config = { 16, 1800, 10.0f, 10000.0f, 50.0f, 15.0f, -15.0f, 360.0f, 2.0f };
		break;

	case ELidarSensorPreset::VelodyneVLP32:
		Config = { 32, 60, 10.0f, 20000.0f, 50.0f, 15.0f, -25.0f, 360.0f, 2.0f };
		break;

	case ELidarSensorPreset::OusterOS1_64:
		Config = { 64, 45, 10.0f, 12000.0f, 50.0f, 22.5f, -22.5f, 360.0f, 1.5f };
		break;

	case ELidarSensorPreset::Livox_Mid360:
		Config = { 8, 45, 10.0f, 7000.0f, 100.0f, 52.0f, -7.0f, 360.0f, 3.0f };
		break;

	case ELidarSensorPreset::Custom:
	default:
		break;
	}
}

void ULidarSceneComponent::SavePointCloudData()
{
	const FString Dir = FPaths::ProjectSavedDir() / TEXT("SensorData") / DataSaveConfig.SensorLabel;
	IFileManager::Get().MakeDirectory(*Dir, true);

	const FString FilePath = Dir / FString::Printf(TEXT("%06lld.bin"), FrameCount);

	TUniquePtr<IFileHandle> File(FPlatformFileManager::Get().GetPlatformFile().OpenWrite(*FilePath));
	if (!File)
	{
		UE_LOG(LogLidarSensor, Warning, TEXT("Failed to open file for writing: %s"), *FilePath);
		return;
	}
	
	//월드 좌표를 센서의 로컬 좌표로 변형한다.
	const FTransform InvSensor = PendingTransform.Inverse();
	const int32 NumPoints = LastPointCloud.PointCount;
	//KITTI는 자율주행 연구용 표준 데이터셋 포맷
	struct FKittiPoint { float X, Y, Z, Intensity; };
	TArray<FKittiPoint> Buffer;
	Buffer.SetNumUninitialized(NumPoints);

	for (int32 i = 0; i < NumPoints; ++i)
	{
		const FVector Local = InvSensor.TransformPosition(LastPointCloud.Points[i]);
		Buffer[i].X         =  static_cast<float>(Local.X * 0.01);
		Buffer[i].Y         = -static_cast<float>(Local.Y * 0.01);
		Buffer[i].Z         =  static_cast<float>(Local.Z * 0.01);
		Buffer[i].Intensity = LastPointCloud.Intensities[i];
	}

	File->Write(
		reinterpret_cast<const uint8*>(Buffer.GetData()),
		NumPoints * sizeof(FKittiPoint)
	);

	UE_LOG(LogLidarSensor, Verbose, TEXT("Saved %d points → %s"), NumPoints, *FilePath);
}
