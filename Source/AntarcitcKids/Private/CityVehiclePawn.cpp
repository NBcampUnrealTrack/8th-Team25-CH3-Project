// CityVehiclePawn.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "CityVehiclePawn.h"
#include "AntarcitcKids/AntarcitcKids.h"
#include "System/AntarcitcKidsPlayerController.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "TimerManager.h"
#include "Component/SplineFollowerComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Sensor/CameraSceneComponent.h"
#include "Sensor/LidarSceneComponent.h"
#include "Sensor/LidarNiagaraComponent.h"
#include "Sensor/BoxBoundComponent.h"

#include "DataLogger/AgentDataLogger.h"
#include "AI/CityVehicleAIController.h"
#include "Kismet/GameplayStatics.h"

ACityVehiclePawn::ACityVehiclePawn()
{
	FrontSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Front Spring Arm"));
	FrontSpringArm->SetupAttachment(GetMesh());
	FrontSpringArm->TargetArmLength = 0.0f;
	FrontSpringArm->bDoCollisionTest = false;
	FrontSpringArm->bEnableCameraRotationLag = true;
	FrontSpringArm->CameraRotationLagSpeed = 15.0f;
	FrontSpringArm->SetRelativeLocation(FVector(30.0f, 0.0f, 120.0f));
	
	FrontCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Front Camera"));
	FrontCamera->SetupAttachment(FrontSpringArm);
	FrontCamera->bAutoActivate = false;
	
	LeftMirrorCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("LeftMirrorCapture"));
	LeftMirrorCapture->SetupAttachment(GetMesh());

	RightMirrorCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("RightMirrorCapture"));
	RightMirrorCapture->SetupAttachment(GetMesh());
	
	BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Back Spring Arm"));
	BackSpringArm->SetupAttachment(GetMesh());
	BackSpringArm->TargetArmLength = 650.0f;
	BackSpringArm->SocketOffset.Z = 150.0f;
	BackSpringArm->bDoCollisionTest = false;
	BackSpringArm->bInheritPitch = false;
	BackSpringArm->bInheritRoll = false;
	BackSpringArm->bEnableCameraRotationLag = true;
	BackSpringArm->CameraRotationLagSpeed = 2.0f;
	BackSpringArm->CameraLagMaxDistance = 50.0f;
	
	BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Back Camera"));
	BackCamera->SetupAttachment(BackSpringArm);
	
	EngineAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineAudio"));
	EngineAudioComponent->SetupAttachment(GetRootComponent());
	EngineAudioComponent->bAutoActivate = false;
	
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName("Vehicle"));
	
	ChaosVehicleMovement = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());
	
	CameraSensor = CreateDefaultSubobject<UCameraSceneComponent>(TEXT("CameraSensor"));
	CameraSensor->SetupAttachment(GetMesh());
	
	LidarSensor = CreateDefaultSubobject<ULidarSceneComponent>(TEXT("LidarSensor"));
	LidarSensor->SetupAttachment(GetMesh());
	LidarSensor->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	
	NiagaraComponent = CreateDefaultSubobject<ULidarNiagaraComponent>(TEXT("LidarNiagaraComponent"));
	NiagaraComponent->SetupAttachment(GetMesh());
	
	BoxBoundComponent = CreateDefaultSubobject<UBoxBoundComponent>(TEXT("BoxBoundComponent"));
	BoxBoundComponent->SetupAttachment(GetMesh());
	
	SplineFollower = CreateDefaultSubobject<USplineFollowerComponent>(TEXT("SplineFollower"));
	
	DataLogger = CreateDefaultSubobject<UAgentDataLogger>(TEXT("DataLogger"));
	
	AIControllerClass = ACityVehicleAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
}

void ACityVehiclePawn::DoSteering(float SteeringValue)
{
	ChaosVehicleMovement->SetSteeringInput(SteeringValue);
}

void ACityVehiclePawn::DoThrottle(float ThrottleValue)
{
	ChaosVehicleMovement->SetThrottleInput(ThrottleValue);
	ChaosVehicleMovement->SetBrakeInput(0.0f);
	BrakeLights(false);
	
	//UE_LOG(LogTemp, Warning, TEXT("Throttle=%.3f"), ThrottleValue);
}

void ACityVehiclePawn::DoFullStop()
{
	bIsManuallyStopped = true;
	
	ChaosVehicleMovement->SetThrottleInput(0.0f);
	ChaosVehicleMovement->SetBrakeInput(1.0f);
	ChaosVehicleMovement->SetHandbrakeInput(true);
	BrakeLights(true);
	
	// 기어 자동 변환을 수동으로
	ChaosVehicleMovement->SetUseAutomaticGears(false);
	ChaosVehicleMovement->SetTargetGear(0, true); // 기어 중립
	
	if (EngineAudioComponent && EngineAudioComponent->IsPlaying())
	{
		EngineAudioComponent->Stop();
	}
}

void ACityVehiclePawn::ResumeMovement()
{
	// 급제동 카메라 용
	bCameraTriggered = false;
	// 강제 정지 상태 해제
	bIsManuallyStopped = false;

	// 브레이크 및 핸드브레이크 해제
	ChaosVehicleMovement->SetBrakeInput(0.0f);
	ChaosVehicleMovement->SetHandbrakeInput(false);
	BrakeLights(false);

	// 자동 변속기 다시 활성화
	ChaosVehicleMovement->SetUseAutomaticGears(true);
    
	// 즉시 출발을 돕기 위해 스로틀 살짝 건듬
	ChaosVehicleMovement->SetThrottleInput(0.1f);
	
}

void ACityVehiclePawn::DoBrake(float BrakeValue)
{
	ChaosVehicleMovement->SetBrakeInput(BrakeValue);
	ChaosVehicleMovement->SetThrottleInput(0.0f);
	BrakeLights(BrakeValue > 0.f);
	
	//UE_LOG(LogTemp, Warning, TEXT("Brake=%.3f"), BrakeValue);
}

void ACityVehiclePawn::DoBrakeStart()
{
	BrakeLights(true);
}

void ACityVehiclePawn::DoBrakeStop()
{
	BrakeLights(false);
	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void ACityVehiclePawn::DoHandbrakeStart()
{
	ChaosVehicleMovement->SetHandbrakeInput(true);
	BrakeLights(true);
}

void ACityVehiclePawn::DoHandbrakeStop()
{
	ChaosVehicleMovement->SetHandbrakeInput(false);
	BrakeLights(false);
}

void ACityVehiclePawn::DoLookAround(float YawDelta)
{
	BackSpringArm->AddLocalRotation(FRotator(0.0f, YawDelta, 0.0f));
}

void ACityVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ResetVehicleAction, ETriggerEvent::Triggered, this, &ACityVehiclePawn::ResetVehicle);
		EnhancedInputComponent->BindAction(ToggleCameraViewAction, ETriggerEvent::Started, this, &ACityVehiclePawn::ToggleSensorView);
		EnhancedInputComponent->BindAction(ToggleLidarViewAction, ETriggerEvent::Started, this, &ACityVehiclePawn::ToggleLidarView);
		EnhancedInputComponent->BindAction(ToggleVisLidarAction, ETriggerEvent::Started, this, &ACityVehiclePawn::ToggleLidarView);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACityVehiclePawn::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(FlipCheckTimer, this, &ACityVehiclePawn::FlippedCheck, FlipCheckTime, true);
	
	
	//여기서 선언된 LidarSensorCOmponent의 장소와 LidarSensorCOmponent의 주소 위치가 달라서 임시 조치를 취함
	/*if (LidarSensor == nullptr)
	{
		LidarSensor = FindComponentByClass<ULidarSceneComponent>();
		
	}*/
	ULidarSceneComponent* RealLidarSensor = FindComponentByClass<ULidarSceneComponent>();
	if (IsValid(RealLidarSensor))
	{
		RealLidarSensor->OnPointCloudReady.AddUObject(NiagaraComponent,&ULidarNiagaraComponent::RenderPointCloudNiagara);
		RealLidarSensor->ImpactActorReady.AddUObject(BoxBoundComponent,&UBoxBoundComponent::RenderBoundingBox);
		/*
		LidarSensor->OnPointCloudReady.AddLambda([](const FLidarPointCloudData& Data) {
		UE_LOG(LogTemp, Error, TEXT("🔥 방송국에서 신호 송신 확인! 데이터 개수: %d"), Data.PointCount);
	});*/
		UE_LOG(LogTemp, Error, TEXT("[Pawn] %p 번지 센서에 바인딩을 걸었습니다!"), LidarSensor.Get());
	}
	else
	{
		UE_LOG(LogTemp,Warning, TEXT("LidarSensor 무력화"));
	}
	
	
	if (ChaosVehicleMovement)
	{
		PreviousGear = ChaosVehicleMovement->GetCurrentGear();
		OnHUDGearUpdated.Broadcast(FText::FromString(TEXT("N")));
	}
	
	if (EngineSound) // 사운드
	{
		EngineAudioComponent->SetSound(EngineSound);
		EngineAudioComponent->Play();
	}
}

void ACityVehiclePawn::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(FlipCheckTimer);
	GetWorld()->GetTimerManager().ClearTimer(MissionTimerHandle);
	
	Super::EndPlay(EndPlayReason);
}

void ACityVehiclePawn::Tick(float Delta)
{
	Super::Tick(Delta);
	
	bool bMovingOnGround = ChaosVehicleMovement->IsMovingOnGround();
	GetMesh()->SetAngularDamping(bMovingOnGround ? 0.0f : 3.0f);

	float CameraYaw = BackSpringArm->GetRelativeRotation().Yaw;
	CameraYaw = FMath::FInterpTo(CameraYaw, 0.0f, Delta, 1.0f);

	BackSpringArm->SetRelativeRotation(FRotator(0.0f, CameraYaw, 0.0f));
	
	// 속도 브로드캐스트 (cm/s → km/h)
	const float SpeedKMH = FMath::Abs(ChaosVehicleMovement->GetForwardSpeed()) * 0.036f;
	const float RoundedSpeed = FMath::IsNearlyZero(SpeedKMH, 0.9f) ? 0.f : 
	FMath::RoundToFloat(SpeedKMH * 10.f) / 10.f;
	OnHUDSpeedUpdated.Broadcast(SpeedKMH);
	
	// RPM 브로드캐스트 (매 프레임)
	const float CurrentRPM = ChaosVehicleMovement->GetEngineRotationSpeed();
	OnHUDRPMUpdated.Broadcast(CurrentRPM);

	FText GearText;
	
	if (bIsManuallyStopped)
	{
		// 강제 정지 중이면 물리 상태와 상관없이 N 고정
		GearText = FText::FromString(TEXT("N"));
	}
	else
	{
		const int32 CurrentGear = ChaosVehicleMovement->GetCurrentGear();
        
		if (CurrentGear < 0)
		{
			GearText = FText::FromString(TEXT("R"));
		}
		else if (CurrentGear == 0)
		{
			GearText = FText::FromString(TEXT("N"));
		}
		else
		{
			GearText = FText::AsNumber(CurrentGear);
		}
	}

	OnHUDGearUpdated.Broadcast(GearText);
	
	UpdateWheelSteerAngleLog(); //실시간으로 바뀌는 바퀴 계산 
	
	if (EngineAudioComponent && EngineAudioComponent->IsPlaying())// 사운드
	{
		//RPM에 따라 소리 조절
		const float RPM = ChaosVehicleMovement->GetEngineRotationSpeed();
		const float MaxRPM = 5500.0f;
		const float PitchMin = 0.5f;
		const float PitchMax = 2.0f;
		const float Pitch = FMath::Lerp(PitchMin, PitchMax, RPM / MaxRPM);
		EngineAudioComponent->SetPitchMultiplier(Pitch);
	}
	
	/* 사운드 체크용
	UE_LOG(LogTemp, Warning, TEXT("RPM=%.0f | IsPlaying=%d"), 
	ChaosVehicleMovement->GetEngineRotationSpeed(),
	EngineAudioComponent->IsPlaying() ? 1 : 0);
	*/
}


void ACityVehiclePawn::UpdateWheelSteerAngleLog()
{
	//UE_LOG(LogTemp, Warning, TEXT("UpdateWheelSteerAngleLog called")); //불러와지는지 체크용
	
	if (!ChaosVehicleMovement)
	{
		return;
	}//비하클 무브먼트가 존재해?
	
	if (ChaosVehicleMovement->Wheels.Num() < 2)
	{
		return;
	}//바퀴가 2개 미만이야?
	
	const UChaosVehicleWheel* FrontLeftWheel = ChaosVehicleMovement->Wheels[0]; //앞, 왼쪽 바퀴 가져오기
	const UChaosVehicleWheel* FrontRightWheel = ChaosVehicleMovement->Wheels[1]; //앞, 오른쪽 바퀴 가져오기
	
	if (!FrontLeftWheel || !FrontRightWheel)
	{
		return;
	} //바퀴 있어?
	
	const double FrontLeftAngle = FrontLeftWheel->GetSteerAngle();
	const double FrontRightAngle = FrontRightWheel->GetSteerAngle(); 
	//실제 조향각 읽기, GetSteerAngle로  값 이동
	
	if (UAgentDataLogger* Logger = FindComponentByClass<UAgentDataLogger>())
	{
		Logger->SetSteeringInputLog(FrontLeftAngle, FrontRightAngle);
	} //로거 찾았어? 몾찼았으면 쓰지마
	
	// HUD 브로드캐스트
	OnHUDSteeringUpdated.Broadcast(
		static_cast<float>(FrontLeftAngle),
		static_cast<float>(FrontRightAngle));
}

//바퀴 조형각을 읽어, 데이터 로그에 전달해주는 함수
//=================================================================================================

//=================================================================================================



void ACityVehiclePawn::LookAround(const FInputActionValue& Value)
{
	DoLookAround(Value.Get<float>());
}

void ACityVehiclePawn::ToggleCamera(const FInputActionValue& Value)
{
	DoToggleCamera();
}

void ACityVehiclePawn::ResetVehicle(const FInputActionValue& Value)
{
	DoResetVehicle();
}

void ACityVehiclePawn::ToggleSensorView(const FInputActionValue& Value)
{
	DoToggleSensorView();
}

void ACityVehiclePawn::ToggleVisLidar(const FInputActionValue& Value)
{
	DoToggleVisLidar();
}

void ACityVehiclePawn::ToggleLidarView(const FInputActionValue& Value)
{
	DoToggleLidarView();
}

void ACityVehiclePawn::DoToggleCamera()
{
	bFrontCameraActive = !bFrontCameraActive;

	FrontCamera->SetActive(bFrontCameraActive);
	BackCamera->SetActive(!bFrontCameraActive);
}

void ACityVehiclePawn::DoResetVehicle()
{
	FVector ResetLocation = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
	FRotator ResetRotation = GetActorRotation();
	ResetRotation.Pitch = 0.0f;
	ResetRotation.Roll = 0.0f;
	
	SetActorTransform(FTransform(ResetRotation, ResetLocation, FVector::OneVector), false, nullptr, ETeleportType::TeleportPhysics);

	GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
}

void ACityVehiclePawn::DoToggleSensorView()
{
	const float UpDot = FVector::DotProduct(FVector::UpVector, GetMesh()->GetUpVector());

	if (UpDot < FlipCheckMinDot)
	{
		if (bPreviousFlipCheck)
			DoResetVehicle();
		
		bPreviousFlipCheck = true;
	}
	else
	{
		bPreviousFlipCheck = false;
	}
}

void ACityVehiclePawn::DoToggleLidarView()
{
	AAntarcitcKidsPlayerController* PC = Cast<AAntarcitcKidsPlayerController>(GetController());
	if (PC == nullptr) return;
	
	//UTextureRenderTarget2D* CamRT = CameraSensor ? CameraSensor->GetRednerTarget() : nullptr;
	//PC->ToggleSensorView(CamRT);
}

void ACityVehiclePawn::DoToggleVisLidar()
{
	UE_LOG(LogTemp,Warning, TEXT("DoToggleVisLidar 발동"));
	LidarSensor->ToggleActive();
}

void ACityVehiclePawn::FlippedCheck()
{
	/*
	AAntarcitcKidsPlayerController* PC = Cast<AAntarcitcKidsPlayerController>(GetController());
	if (PC == nullptr) return;
		
	UTexture2D* LidarRT = LidarSensor ? LidarSensor->GetBevRenderTarget() : nullptr;
	PC->ToggleLidarView(LidarRT); 
	
	if (LidarSensor)
	{
		if (PC->IsLidarViewVisible())
			LidarSensor->StartScan();
		else
			LidarSensor->StopScan();
	}
	*/
}

void ACityVehiclePawn::CompleteMission(int32 MissionIndex, FName QuestName)
{
	OnHUDMissionUpdated.Broadcast(MissionIndex, true, QuestName);
}

void ACityVehiclePawn::RegisterMission(int32 MissionIndex, FName QuestName)
{
	OnHUDMissionRegistered.Broadcast(MissionIndex, QuestName);
}

void ACityVehiclePawn::StartMissionTimer(float TotalSeconds)
{
	MissionTimeRemaining = TotalSeconds;
	GetWorld()->GetTimerManager().ClearTimer(MissionTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(
		MissionTimerHandle,
		this,
		&ACityVehiclePawn::TickMissionTimer,
		1.0f,
		true);
	OnHUDTimerUpdated.Broadcast(MissionTimeRemaining);
}

void ACityVehiclePawn::TickMissionTimer()
{
	MissionTimeRemaining = FMath::Max(0.f, MissionTimeRemaining - 1.f);
	OnHUDTimerUpdated.Broadcast(MissionTimeRemaining);

	if (MissionTimeRemaining <= 0.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(MissionTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("[CyberHUD] Mission timer expired!"));
	}
}

void ACityVehiclePawn::TriggerEmergencyBrakeCamera(float BrakeValue)
{
	if (bCameraTriggered) return;
	
	bCameraTriggered = true;
	
	OnEmergencyBrake.Broadcast(BrakeValue);
	
	// 브레이크 사운드
	if (BrakeSounds.Num() > 0)
	{
		int32 Index = FMath::RandRange(0, BrakeSounds.Num() -1);
		UGameplayStatics::PlaySoundAtLocation(this, BrakeSounds[Index], GetActorLocation());
	}
}