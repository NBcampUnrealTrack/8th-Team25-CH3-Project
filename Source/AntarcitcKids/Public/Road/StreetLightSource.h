
#pragma once

#include "CoreMinimal.h"
#include "Light/LightSourceBase.h"
#include "StreetLightSource.generated.h"

class USpotLightComponent;

UCLASS()
class ANTARCITCKIDS_API AStreetLightSource : public ALightSourceBase
{
	GENERATED_BODY()
	
public:	
	AStreetLightSource();
	

	virtual void SetLightEnabled(bool bEnabled) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Street Light")
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Street Light")
	TObjectPtr<USpotLightComponent>	SpotLight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Street Light|Settings")
	bool bUseSpotLight = true;
	
	// 빛의 강도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Street Light|Settings")
	float SpotLightIntensity = 8000.f;
	
	//빛의 반경
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Street Light|Settings")
	float AttenuationRadius = 800.f;
	
	//빛 색상
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Street Light|Settings")
	FLinearColor LightColor = FLinearColor(1.f, 0.82f, 0.55f);
};
