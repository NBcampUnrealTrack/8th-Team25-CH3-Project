// SensorSubSystem.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sensor/CameraSensorTypes.h"
#include "SensorSubSystem.generated.h"


/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSensorPresetChanged, ELidarSensorPreset)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCameraPresetChanged, ECameraSensorPreset)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSetSensorHz, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSetCameraHz, float)


UCLASS()
class ANTARCITCKIDS_API USensorSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;
	
	
	UFUNCTION(BlueprintCallable)
	void LidarSenserPresetChange(ELidarSensorPreset SelectedSensor);
	
	UFUNCTION(BlueprintCallable)
	void CameraPresetChange(ECameraSensorPreset CameraSensorPreset);
	
	UFUNCTION(BlueprintCallable)
	void CameraHzChange(float Hz);
	
	UFUNCTION(BlueprintCallable)
	void LidarSensorHzChange(float Hz);
	
public:
	FOnSensorPresetChanged OnSensorPresetChanged;
	FOnCameraPresetChanged OnCameraPresetChanged;
	FOnSetSensorHz OnSetSensorHz;
	FOnSetCameraHz OnSetCameraHz;
	
	ELidarSensorPreset GetCurrentLidarSensorPreset() {return CurrentLidarSensorPreset;}
	ECameraSensorPreset GetCurrentCameraSensorPreset() {return CurrentCameraSensorPreset;}
	float GetCurrentLidarSensorHz() {return CurrentLidarSensorHz;}
	float GetCurrentCameraSensorHz() {return CurrentCameraSensorHz;}
	
	
	
private:
	ELidarSensorPreset CurrentLidarSensorPreset;
	

	float CurrentLidarSensorHz;
	

	ECameraSensorPreset CurrentCameraSensorPreset;
	

	float CurrentCameraSensorHz;
	
};
