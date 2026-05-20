
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Light")
	TObjectPtr<ADirectionalLight> DirectionalLight;
	
	UFUNCTION(CallInEditor, Category = "Light")
	void UpdateLights();
	
};
