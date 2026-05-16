// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/SensorSubSystem.h"

void USensorSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USensorSubSystem::Deinitialize()
{
	Super::Deinitialize();
	
}

void USensorSubSystem::LidarSenserPresetChange(ELidarSensorPreset SelectedSensor)
{
	CurrentLidarSensorPreset = SelectedSensor;
	OnSensorPresetChanged.Broadcast(SelectedSensor);
}

void USensorSubSystem::CameraPresetChange(ECameraSensorPreset CameraSensorPreset)
{
	CurrentCameraSensorPreset = CameraSensorPreset;
	OnCameraPresetChanged.Broadcast(CameraSensorPreset);
}

void USensorSubSystem::CameraHzChange(float Hz)
{
	CurrentCameraSensorHz = Hz;
	OnSetCameraHz.Broadcast(Hz);
}

void USensorSubSystem::LidarSensorHzChange(float Hz)
{
	CurrentLidarSensorHz = Hz;
	OnSetSensorHz.Broadcast(Hz);
}





