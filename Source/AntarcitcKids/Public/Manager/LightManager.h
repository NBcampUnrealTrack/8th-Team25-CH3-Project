#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Manager/TimeSubsystem.h"
#include "LightManager.generated.h"

class ADirectionalLight;
class ALightSourceBase;

UCLASS()
class ANTARCITCKIDS_API ALightManager : public AActor
{
	GENERATED_BODY()

public:
	ALightManager();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void UpdateLights(double Pitch, FTimeOfDay TimeData);
	
	UPROPERTY()
	TArray<TObjectPtr<ALightSourceBase>> ManagedLights;
};