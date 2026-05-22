// AntarcitcKidsPlayerController.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "System/AntarcitcKidsPlayerController.h"
#include "CityVehiclePawn.h"
#include "Sensor/SensorViewrWidget.h"
#include "Manager/QuestSubSystem.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerStart.h"
#include "QuestCameraWidget.h"
#include "Camera/CameraComponent.h"
#include "Chaos/SoftsSpring.h"
#include "SimPauseWidget.h"
#include "Manager/SimControlSubsystem.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"



// 커스텀 로그 카테고리를 정의
DEFINE_LOG_CATEGORY_STATIC(LogAKPlayerController, Log, All);

void AAntarcitcKidsPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	bAttachToPawn = true;
	
	// 한 프레임 대기 후 차량 찾기 (차량 스폰되는 시간 주기)
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(
		Handle, this, &AAntarcitcKidsPlayerController::FindAndBindVehicle, 0.1f, false);

	/*
	if (SensorViewWidgetClass)
	{
		SensorViewWidget = CreateWidget<USensorViewWidget>(this, SensorViewWidgetClass);
		if (SensorViewWidget)
		{
			SensorViewWidget->AddToViewport(10);
			SensorViewWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}*/
}

void AAntarcitcKidsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}
	
	// Pause
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (PauseAction)
			EIC->BindAction(PauseAction, ETriggerEvent::Started, this, &AAntarcitcKidsPlayerController::OnPauseTriggered);
	}
}

void AAntarcitcKidsPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AAntarcitcKidsPlayerController::OnPawnDestroyed(AActor* DestroyedPawn)
{
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), ActorList);

	if (ActorList.Num() > 0)
	{
		const FTransform SpawnTransform = ActorList[0]->GetActorTransform();

		if (ACityVehiclePawn* RespawnedVehicle = GetWorld()->SpawnActor<ACityVehiclePawn>(VehiclePawnClass, SpawnTransform))
		{
			// Possess 안 함! AI Controller가 자동 빙의함
			// 대신 카메라/HUD 다시 바인딩
			VehiclePawn  = RespawnedVehicle;
			SetViewTargetWithBlend(VehiclePawn);
			VehiclePawn->OnDestroyed.AddDynamic(this, &AAntarcitcKidsPlayerController::OnPawnDestroyed);
			CreateAndBindCyberHUD();
		}
	}
}

void AAntarcitcKidsPlayerController::FindAndBindVehicle()
{
	TArray<AActor*> Vehicles;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACityVehiclePawn::StaticClass(), Vehicles);
	if (Vehicles.Num() > 0)
	{
		VehiclePawn = Cast<ACityVehiclePawn>(Vehicles[0]);
		if (VehiclePawn)
		{
			// PlayerController의 카메라를 VehiclePawn 시점으로 부드럽게 전환
			SetViewTargetWithBlend(VehiclePawn);
			
			UCameraComponent* CameraComponent = VehiclePawn->GetBackCamera();
			USpringArmComponent* SpringArmComponent = VehiclePawn->GetBackSpringArm();
	
			if (IsValid(CameraComponent) && IsValid(SpringArmComponent))
			{
				DefaultRotator = SpringArmComponent->GetComponentRotation();
				DefaultDepthOfFieldFocalDistance = CameraComponent->PostProcessSettings.DepthOfFieldFocalDistance;
		
			}
			
			
			VehiclePawn->OnDestroyed.AddDynamic(this, &AAntarcitcKidsPlayerController::OnPawnDestroyed);
			
			// HUD 바인딩
			CreateAndBindCyberHUD();
			
			if (UQuestSubSystem* QuestSub = GetGameInstance()->GetSubsystem<UQuestSubSystem>())
			{
				QuestSub->SetVehiclePawn(VehiclePawn);
			}
		}
		else
		{
			UE_LOG(LogAKPlayerController, Warning, TEXT("PlayerController: No ACityVehiclePawn found in world."));
		}
	}
	
	
}

void AAntarcitcKidsPlayerController::HighLightActor(USceneCaptureComponent2D* TargetActor)
{
	UCameraComponent* CameraComponent = VehiclePawn->GetBackCamera();
	USpringArmComponent* SpringArmComponent = VehiclePawn->GetBackSpringArm();
	
	
	
	
	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(
	CameraComponent->GetComponentLocation(),
	TargetActor->GetComponentLocation()+FVector(0.f, 0.f, 0.f));
	/*UE_LOG(LogTemp,Warning, TEXT("LookAt 활성화"));*/
	
	
		
	float Distance = FVector::Dist(
		TargetActor->GetComponentLocation(), 
		VehiclePawn->GetActorLocation());
	
	UE_LOG(LogTemp, Warning, TEXT("HightLightActor 발동"));

	
	FRotator Smoothed = FMath::RInterpTo(
		 CameraComponent->GetComponentRotation(),
		 LookAt,
		 GetWorld()->GetDeltaSeconds(),
		 1.f);
	CameraComponent->SetWorldRotation(Smoothed);
	
	
	CameraComponent->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
	CameraComponent->PostProcessSettings.DepthOfFieldFocalDistance = Distance;
	
}

void AAntarcitcKidsPlayerController::ResetHightlight()
{
	UCameraComponent* CameraComponent = VehiclePawn->GetBackCamera();
	USpringArmComponent* SpringArmComponent = VehiclePawn->GetBackSpringArm();
	
	if (!CameraComponent || !SpringArmComponent) return;
	
	FRotator Smoothed = FMath::RInterpTo(
	 CameraComponent->GetComponentRotation(),
	 DefaultRotator,
	 GetWorld()->GetDeltaSeconds(),
	 1.f);
	CameraComponent->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = false;
	CameraComponent->PostProcessSettings.DepthOfFieldFocalDistance = DefaultDepthOfFieldFocalDistance;
}


void AAntarcitcKidsPlayerController::ToggleSensorView(UTextureRenderTarget2D* InCameraRT)
{
	
	/*if (!SensorViewWidget) return;

	if (InCameraRT)
	{
		SensorViewWidget->SetRenderTarget(InCameraRT);
	}
	SensorViewWidget->ToggleCameraView();*/
	
}



void AAntarcitcKidsPlayerController::TurnOnQuestCameraView(UTextureRenderTarget2D* InCameraRT)
{
	if (!QuestCameraWidget) return;
	UE_LOG(LogTemp,Warning, TEXT("ToogleQuestCameraView 작동됨"));
	if (InCameraRT)
	{
		UE_LOG(LogTemp,Warning, TEXT("InCameraRT 있음"));
		QuestCameraWidget->SetRenderTarget(InCameraRT);
	}
	else
	{
		UE_LOG(LogTemp,Warning, TEXT("InCameraRT 없음"));
	}
	
	QuestCameraWidget->TurnOnCameraView();
}

void AAntarcitcKidsPlayerController::TurnOffQuestCameraView(UTextureRenderTarget2D* InCameraRT)
{
	if (!QuestCameraWidget) return;
	UE_LOG(LogTemp,Warning, TEXT("ToogleQuestCameraView 작동됨"));
	if (InCameraRT)
	{
		UE_LOG(LogTemp,Warning, TEXT("InCameraRT 있음"));
		QuestCameraWidget->SetRenderTarget(InCameraRT);
	}
	else
	{
		UE_LOG(LogTemp,Warning, TEXT("InCameraRT 없음"));
	}
	
	QuestCameraWidget->TurnOffCameraView();
}

void AAntarcitcKidsPlayerController::ToggleLidarView(UTexture2D* InLidarRT)
{
	/*
	if (!SensorViewWidget) return;

	if (InLidarRT)
	{
		SensorViewWidget->SetLidarRenderTarget(InLidarRT);
	}
	SensorViewWidget->ToggleLidarView();
	*/
}




bool AAntarcitcKidsPlayerController::IsLidarViewVisible() const
{
	return true;
}


//=======================CyberHUD=========================

void AAntarcitcKidsPlayerController::CreateAndBindCyberHUD()
{
	if (!CyberHUDWidgetClass || !VehiclePawn) return;

	if (CyberHUDWidget)
	{
		CyberHUDWidget->RemoveFromParent();
		CyberHUDWidget = nullptr;
	}

	CyberHUDWidget = CreateWidget<UUserWidget>(this, CyberHUDWidgetClass);
	if (CyberHUDWidget)
	{
		CyberHUDWidget->AddToViewport();
	}
	
	if (QuestCameraWidgetClass)
	{
		QuestCameraWidget = CreateWidget<UQuestCameraWidget>(this, QuestCameraWidgetClass);
		if (QuestCameraWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Viewport 추가됨"));
			QuestCameraWidget->AddToViewport(10);
			
		}
	}

	VehiclePawn->OnHUDSpeedUpdated.AddDynamic(this,    &AAntarcitcKidsPlayerController::OnHUDSpeedUpdated);
	VehiclePawn->OnHUDSteeringUpdated.AddDynamic(this, &AAntarcitcKidsPlayerController::OnHUDSteeringUpdated);
	VehiclePawn->OnHUDMissionUpdated.AddDynamic(this,  &AAntarcitcKidsPlayerController::OnHUDMissionUpdated);
	VehiclePawn->OnHUDMissionRegistered.AddDynamic(this, &AAntarcitcKidsPlayerController::OnHUDMissionRegistered);
	VehiclePawn->OnHUDTimerUpdated.AddDynamic(this,    &AAntarcitcKidsPlayerController::OnHUDTimerUpdated);
	VehiclePawn->OnHUDGearUpdated.AddDynamic(this, &AAntarcitcKidsPlayerController::OnHUDGearUpdated);
	VehiclePawn->OnHUDRPMUpdated.AddDynamic(this,  &AAntarcitcKidsPlayerController::OnHUDRPMUpdated);
}

void AAntarcitcKidsPlayerController::OnHUDSpeedUpdated(float SpeedKMH)
{
	
}
void AAntarcitcKidsPlayerController::OnHUDSteeringUpdated(float LeftAngle, float RightAngle)
{
	
}
void AAntarcitcKidsPlayerController::OnHUDMissionUpdated(int32 MissionIndex, bool bCompleted, FName QuestName)
{
	if (!CyberHUDWidget) return;

	UFunction* Func = CyberHUDWidget->FindFunction(FName("OnMissionCompleted"));
	if (Func)
	{
		struct { int32 MissionIndex; bool bCompleted; FName QuestName; } Params = { MissionIndex, bCompleted, QuestName };
		CyberHUDWidget->ProcessEvent(Func, &Params);
	}
}
void AAntarcitcKidsPlayerController::OnHUDMissionRegistered(int32 MissionIndex, FName QuestName)
{
	if (!CyberHUDWidget) return;

	UFunction* Func = CyberHUDWidget->FindFunction(FName("OnMissionRegistered"));
	if (Func)
	{
		struct { int32 MissionIndex; FName QuestName; } Params = { MissionIndex, QuestName };
		CyberHUDWidget->ProcessEvent(Func, &Params);
	}
}
void AAntarcitcKidsPlayerController::OnHUDTimerUpdated(float RemainingSeconds)
{
	
}
void AAntarcitcKidsPlayerController::OnHUDGearUpdated(FText GearText)
{
	
}
void AAntarcitcKidsPlayerController::OnHUDRPMUpdated(float CurrentRPM)
{
	
}

void AAntarcitcKidsPlayerController::OnPauseTriggered()
{
	if (PauseWidget && PauseWidget->IsInViewport())
	{
		PauseWidget->CloseWidget();
		return;
	}

	if (USimControlSubsystem* Ctrl = GetGameInstance()->GetSubsystem<USimControlSubsystem>())
		Ctrl->Pause();

	if (!PauseWidget && PauseWidgetClass)
		PauseWidget = CreateWidget<USimPauseWidget>(this, PauseWidgetClass);

	if (PauseWidget)
	{
		PauseWidget->AddToViewport(10);
		SetShowMouseCursor(true);
		SetInputMode(FInputModeUIOnly());
	}
}