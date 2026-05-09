#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AntarcitcKidsPlayerController.generated.h"

class UInputMappingContext;
class ACityVehiclePawn;
class USensorViewrWidget;
class UTextureRenderTarget2D;

UCLASS()
class ANTARCITCKIDS_API AAntarcitcKidsPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void OnPawnDestroyed(AActor* DestroyedPawn);

public:
	void ToggleSensorView(UTextureRenderTarget2D* InCameraRT);
	void ToggleLidarView(UTexture2D* InLidarRT);
	bool IsLidarViewVisible() const;

private:
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	UPROPERTY(EditAnywhere, Category = "Input|Steering Wheel Controls")
	bool bUseSteeringWheelControls = false;

	UPROPERTY(EditAnywhere, Category = "Input|Steering Wheel Controls", meta = (EditCondition = "bUseSteeringWheelControls"))
	UInputMappingContext* SteeringWheelInputMappingContext;

	UPROPERTY(EditAnywhere, Category="Vehicle|Respawn")
	TSubclassOf<ACityVehiclePawn> VehiclePawnClass;

	UPROPERTY(EditAnywhere, Category="Vehicle|UI")
	TSubclassOf<USensorViewrWidget> SensorViewWidgetClass;

	UPROPERTY()
	TObjectPtr<ACityVehiclePawn> VehiclePawn;

	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	UPROPERTY()
	TObjectPtr<USensorViewrWidget> SensorViewWidget;
};
