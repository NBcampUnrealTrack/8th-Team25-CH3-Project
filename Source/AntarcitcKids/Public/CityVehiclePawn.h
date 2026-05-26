// CityVehiclePawn.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/AudioComponent.h"
#include "CityVehiclePawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHUDSpeedUpdated, float, SpeedKMH);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHUDSteeringUpdated, float, LeftAngle, float, RightAngle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHUDMissionUpdated, int32, MissionIndex, bool, bCompleted, FName, QuestName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHUDMissionRegistered, int32, MissionIndex, FName, QuestName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHUDTimerUpdated, float, RemainingSeconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHUDGearUpdated, FText, GearText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHUDRPMUpdated, float, CurrentRPM);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEmergencyBrake, float, BrakeValue);// 급정거 카메라 무빙
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHUDTireTemperatureUpdated, float, TempFL, float, TempFR, float, TempRL, float, TempRR);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHUDPedalUpdated, float, ThrottleValue, float, BrakeValue);

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UChaosWheeledVehicleMovementComponent;
class UCameraSceneComponent;  
class ULidarSceneComponent;
class UCameraSensorComponent;
class ULidarSensorComponent;
class ULidarNiagaraComponent;
class UBoxBoundComponent;
class USplineFollowerComponent;
class UAgentDataLogger;
struct FInputActionValue;


UCLASS()
class ANTARCITCKIDS_API ACityVehiclePawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()
	
public:
	ACityVehiclePawn();
	
	// 급정거 카메라 무빙
	UPROPERTY(BlueprintAssignable, Category="Camera")
	FOnEmergencyBrake OnEmergencyBrake;
	
	UFUNCTION(BlueprintCallable, Category="Camera")
	void TriggerEmergencyBrakeCamera(float BrakeValue);
	
	// 급정거 사운드
	UPROPERTY(EditAnywhere, Category="Sound")
	TArray<TObjectPtr<USoundBase>> BrakeSounds;
	
	// 사운드
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Sound", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAudioComponent> EngineAudioComponent;

	UPROPERTY(EditAnywhere, Category="Sound")
	TObjectPtr<USoundBase> EngineSound;
	
	// HUD 델리게이트
	UPROPERTY(BlueprintAssignable, Category="HUD")
	FOnHUDSpeedUpdated OnHUDSpeedUpdated;

	UPROPERTY(BlueprintAssignable, Category="HUD")
	FOnHUDSteeringUpdated OnHUDSteeringUpdated;

	UPROPERTY(BlueprintAssignable, Category="HUD")
	FOnHUDMissionUpdated OnHUDMissionUpdated;

	UPROPERTY(BlueprintAssignable, Category="HUD")
	FOnHUDMissionRegistered OnHUDMissionRegistered;
	
	UPROPERTY(BlueprintAssignable, Category="HUD")
	FOnHUDTimerUpdated OnHUDTimerUpdated;

	UPROPERTY(BlueprintAssignable, Category="HUD")
	FOnHUDGearUpdated OnHUDGearUpdated;

	UPROPERTY(BlueprintAssignable, Category="HUD")
	FOnHUDRPMUpdated OnHUDRPMUpdated;
	
	UPROPERTY(BlueprintAssignable, Category="HUD")
	FOnHUDTireTemperatureUpdated OnHUDTireTemperatureUpdated;
	
	UPROPERTY(BlueprintAssignable, Category="HUD")
	FOnHUDPedalUpdated OnHUDPedalUpdated;
	
	// HUD API
	UFUNCTION(BlueprintCallable, Category="HUD|Mission")
	void CompleteMission(int32 MissionIndex, FName QuestName);
	
	UFUNCTION(BlueprintCallable, Category="HUD|Mission")
	void RegisterMission(int32 MissionIndex, FName QuestName);

	UFUNCTION(BlueprintCallable, Category="HUD|Timer")
	void StartMissionTimer(float TotalSeconds);
	
	//======================================================
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoSteering(float SteeringValue);

	UFUNCTION(BlueprintCallable, Category="Input")
	void DoThrottle(float ThrottleValue);
	
	// 완전 정차
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoFullStop();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void ResumeMovement();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoBrake(float BrakeValue);

	UFUNCTION(BlueprintCallable, Category="Input")
	void DoBrakeStart();

	UFUNCTION(BlueprintCallable, Category="Input")
	void DoBrakeStop();

	UFUNCTION(BlueprintCallable, Category="Input")
	void DoHandbrakeStart();

	UFUNCTION(BlueprintCallable, Category="Input")
	void DoHandbrakeStop();

	UFUNCTION(BlueprintCallable, Category="Input")
	void DoLookAround(float YawDelta);
	
	FORCEINLINE USpringArmComponent* GetFrontSpringArm() const { return FrontSpringArm; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FrontCamera; }
	FORCEINLINE USpringArmComponent* GetBackSpringArm() const { return BackSpringArm; }
	FORCEINLINE UCameraComponent* GetBackCamera() const { return BackCamera; }
	FORCEINLINE UChaosWheeledVehicleMovementComponent* GetChaosVehicleMovement() const { return ChaosVehicleMovement; }
	/*//FORCEINLINE UCameraSensorComponent* GetCameraSensor() const { return CameraSensor; }
	//FORCEINLINE ULidarSensorComponent* GetLidarSensor() const { return LidarSensor; }*/
	FORCEINLINE UAgentDataLogger* GetDataLogger() const { return DataLogger; }
	
	UFUNCTION(BlueprintPure, Category="Vehicle")
	float GetForwardSpeed() const{ return FVector::DotProduct(GetVelocity(), GetActorForwardVector()); }
	
	UFUNCTION(BlueprintPure, Category="Vehicle")
	float GetCurrentSpeedKMH() const{ return FMath:: Abs(GetForwardSpeed()) * 0.036f; } // cm/s → km/h 변환
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoToggleVisLidar();
	
	ULidarNiagaraComponent* GetNiagaraComponent() {return NiagaraComponent;}
	UBoxBoundComponent* GetBoxBoundComponent() { return BoxBoundComponent;}
	
protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float Delta) override;
	
	void UpdateWheelSteerAngleLog(); //로그로 값을 보내기 위한 값 추가
	
private:
	void LookAround(const FInputActionValue& Value);
	void ToggleCamera(const FInputActionValue& Value);
	void ResetVehicle(const FInputActionValue& Value);
	void ToggleSensorView(const FInputActionValue& Value);
	void ToggleVisLidar(const FInputActionValue& Value);
	void ToggleLidarView(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoToggleCamera();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoResetVehicle();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoToggleSensorView();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoToggleLidarView();
	

	
	void UpdateTireTemperatures(float DeltaTime);
	
protected:
	
	UFUNCTION(BlueprintImplementableEvent, Category="Vehicle")
	void BrakeLights(bool bBraking);
	
	UFUNCTION()
	void FlippedCheck();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tire")
	float TireTempFL;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tire")
	float TireTempFR;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tire")
	float TireTempRL;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tire")
	float TireTempRR;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> FrontSpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FrontCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> BackSpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> BackCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USceneCaptureComponent2D> LeftMirrorCapture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USceneCaptureComponent2D> RightMirrorCapture;

	UPROPERTY(EditAnywhere, Category="Mirror")
	TObjectPtr<UTextureRenderTarget2D> LeftMirrorRenderTarget;

	UPROPERTY(EditAnywhere, Category="Mirror")
	TObjectPtr<UTextureRenderTarget2D> RightMirrorRenderTarget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraSceneComponent> CameraSensor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULidarSceneComponent> LidarSensor;  
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULidarNiagaraComponent> NiagaraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxBoundComponent> BoxBoundComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAgentDataLogger> DataLogger;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineFollowerComponent> SplineFollower;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UChaosWheeledVehicleMovementComponent> ChaosVehicleMovement;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ResetVehicleAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ToggleCameraViewAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ToggleLidarViewAction;
	
	
	UPROPERTY(EditAnywhere, Category="Flip Check", meta = (Units = "s"))
	float FlipCheckTime = 3.0f;
	
	UPROPERTY(EditAnywhere, Category="Flip Check")
	float FlipCheckMinDot = -0.2f;
	
	bool bFrontCameraActive = false;
	bool bPreviousFlipCheck = false;
	bool bIsManuallyStopped = false;
	bool bCameraTriggered = false;
	
	UFUNCTION()
	void TickMissionTimer();

	FTimerHandle MissionTimerHandle;
	float MissionTimeRemaining = 0.f;
	
	FTimerHandle FlipCheckTimer;
	
	int32 PreviousGear = 0;
};
