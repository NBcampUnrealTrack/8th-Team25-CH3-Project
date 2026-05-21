
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightManager.generated.h"

class ADirectionalLight;

UCLASS()
class ANTARCITCKIDS_API ALightManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ALightManager();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(Category = "Light")
	void UpdateLights(double Pitch);
	
};
