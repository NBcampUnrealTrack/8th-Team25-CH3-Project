
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StreetLightSource.generated.h"

class USpotLightComponent;

UCLASS()
class ANTARCITCKIDS_API AStreetLightSource : public AActor
{
	GENERATED_BODY()
	
public:	
	AStreetLightSource();

	virtual void OnConstruction(const FTransform& Transform) override;
	
	UFUNCTION(category="Street Light")
	void SetLightEnabled(bool bEnabled) const;
	
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
