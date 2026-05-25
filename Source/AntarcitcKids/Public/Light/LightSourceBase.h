#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightSourceBase.generated.h"

UCLASS()
class ANTARCITCKIDS_API ALightSourceBase : public AActor
{
	GENERATED_BODY()

public:
	ALightSourceBase();

	UFUNCTION(BlueprintCallable, Category = "Light")
	virtual void SetLightEnabled(bool bEnabled);
};