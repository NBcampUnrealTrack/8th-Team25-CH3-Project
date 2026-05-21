#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SimControlSubsystem.generated.h"

UENUM(BlueprintType)
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
	
public:
	UFUNCTION(BlueprintCallable)
	void Pause();
	UFUNCTION(BlueprintCallable)
	void Play();
	UFUNCTION(BlueprintCallable)
	void SetAccelation(float TimeDliation);
	UFUNCTION(BlueprintCallable)
	void TogglePlayPause();
	
private:
	ESimulationState CurrentState = ESimulationState::Play;
	float CurrentTimeDliation = 1.f;
	
	
};
