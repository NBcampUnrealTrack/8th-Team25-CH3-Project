// SensorSubSystem.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Subsystem/SensorSubSystem.h"

void USensorSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USensorSubSystem::Deinitialize()
{
	Super::Deinitialize();
	
}

//라이더 센서의 프리셋 변경시 델리게이트 작동
void USensorSubSystem::LidarSenserPresetChange(ELidarSensorPreset SelectedSensor)
{
	CurrentLidarSensorPreset = SelectedSensor;
	OnSensorPresetChanged.Broadcast(SelectedSensor);
}

//카메라 센서의 프리셋 변경시 델리게이트 작동
void USensorSubSystem::CameraPresetChange(ECameraSensorPreset CameraSensorPreset)
{
	CurrentCameraSensorPreset = CameraSensorPreset;
	OnCameraPresetChanged.Broadcast(CameraSensorPreset);
}

//카메라 센서의 초당 촬영횟수 변경시 델리게이트 작동
void USensorSubSystem::CameraHzChange(float Hz)
{
	CurrentCameraSensorHz = Hz;
	OnSetCameraHz.Broadcast(Hz);
}


//라이다 센서의 초당 촬영횟수 변경시 델리게이트 작동
void USensorSubSystem::LidarSensorHzChange(float Hz)
{
	CurrentLidarSensorHz = Hz;
	OnSetSensorHz.Broadcast(Hz);
}





