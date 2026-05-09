#include "System/AntarcitcKidsPlayerController.h"
#include "CityVehiclePawn.h"
#include "Sensor/SensorViewrWidget.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

void AAntarcitcKidsPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	bAttachToPawn = true;

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
}

void AAntarcitcKidsPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	VehiclePawn = CastChecked<ACityVehiclePawn>(InPawn);
	VehiclePawn->OnDestroyed.AddDynamic(this, &AAntarcitcKidsPlayerController::OnPawnDestroyed);
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
			Possess(RespawnedVehicle);
		}
	}
}

void AAntarcitcKidsPlayerController::ToggleSensorView(UTextureRenderTarget2D* InCameraRT)
{
	/*
	if (!SensorViewWidget) return;

	if (InCameraRT)
	{
		SensorViewWidget->SetRenderTarget(InCameraRT);
	}
	SensorViewWidget->ToggleCameraView();
	*/
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
/*
bool AAntarcitcKidsPlayerController::IsLidarViewVisible() const
{
	return SensorViewWidget && SensorViewWidget->IsLidarViewVisible();
}*/