#pragma once

#include "CoreMinimal.h"
#include "Light/LightSourceBase.h"
#include "PointLightSource.generated.h"

class USceneComponent;
class UPointLightComponent;

UCLASS()
class ANTARCITCKIDS_API APointLightSource : public ALightSourceBase
{
	GENERATED_BODY()

public:
	APointLightSource();

	virtual void SetLightEnabled(bool bEnabled) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPointLightComponent> PointLight;
};