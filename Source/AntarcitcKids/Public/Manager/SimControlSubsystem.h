// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SimControlSubsystem.generated.h"

/**
 * 
 */
enum class ESimulationState : uint8
{
	Pause UMETA(DisplayName = "Pause"),
	Play UMETA(DisplayName = "Play"),
	Accelation UMETA(DisplayName = "Accelation")
};


UCLASS()
class ANTARCITCKIDS_API USimControlSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	void Pause();
	void Play();
	void SetAccelation(float TimeDliation);
	void TogglePlayPause();
	
private:
	ESimulationState CurrentState;
	float CurrentTimeDliation;
	
	
};
