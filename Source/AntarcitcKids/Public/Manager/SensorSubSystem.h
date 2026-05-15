// SensorSubSystem.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SensorSubSystem.generated.h"

UCLASS()
class ANTARCITCKIDS_API USensorSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;
};
