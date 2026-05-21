// AntarcitcKidsPlayerController.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "AntarcitcKidsPlayerController.generated.h"

class UInputMappingContext;
class ACityVehiclePawn;
class USensorViewrWidget;
class UTextureRenderTarget2D;
class USpringArmComponent;
class UQuestCameraWidget;
class USimPauseWidget;


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
	
	// 신규: 월드의 차량을 찾아 참조 획득
	UFUNCTION()
	void FindAndBindVehicle();

public:
	
	void HighLightActor(USceneCaptureComponent2D* TargetActor);
	void ResetHightlight();
	
	void ToggleSensorView(UTextureRenderTarget2D* InCameraRT);
	void TurnOnQuestCameraView(UTextureRenderTarget2D* InCameraRT);
	void TurnOffQuestCameraView(UTextureRenderTarget2D* InCameraRT);
	
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
	
	//QuestCamera 관련

	UPROPERTY(EditAnywhere ,Category = "Vehicle|UI")
	TSubclassOf<UQuestCameraWidget> QuestCameraWidgetClass;
	
	UPROPERTY(EditAnywhere ,Category = "Vehicle|UI")
	TObjectPtr<UQuestCameraWidget> QuestCameraWidget;
	
	bool IsPlayerOrTriggerActor;
	
	float DefaultDepthOfFieldFocalDistance;
	FRotator DefaultRotator;
	
	// CyberHUD담당
	UPROPERTY(EditAnywhere, Category="Vehicle|UI")
	TSubclassOf<UUserWidget> CyberHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> CyberHUDWidget;
	
	// Pause 담당
	UPROPERTY(EditAnywhere, Category = "UI|Pause")
	TObjectPtr<UInputAction> PauseAction;

	UPROPERTY(EditAnywhere, Category = "UI|Pause")
	TSubclassOf<USimPauseWidget> PauseWidgetClass;

	UPROPERTY()
	TObjectPtr<USimPauseWidget> PauseWidget;

	UFUNCTION()
	void OnPauseTriggered();

	void CreateAndBindCyberHUD();

	UFUNCTION() 
	void OnHUDSpeedUpdated(float SpeedKMH);
	UFUNCTION() 
	void OnHUDSteeringUpdated(float LeftAngle, float RightAngle);
	UFUNCTION() 
	void OnHUDMissionUpdated(int32 MissionIndex, bool bCompleted, FName QuestName);
	UFUNCTION()
	void OnHUDMissionRegistered(int32 MissionIndex, FName QuestName);
	UFUNCTION() 
	void OnHUDTimerUpdated(float RemainingSeconds);
	UFUNCTION() 
	void OnHUDGearUpdated(FText GearText);
	UFUNCTION() 
	void OnHUDRPMUpdated(float CurrentRPM);
};
