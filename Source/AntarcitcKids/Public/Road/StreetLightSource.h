
#pragma once

#include "CoreMinimal.h"
#include "Light/LightSourceBase.h"
#include "StreetLightSource.generated.h"

class USpotLightComponent;
class UStaticMeshComponent;

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
	
	//가로등 켜지는게 눈에 잘 안보이길래  메쉬 추가
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Street Light")
	TObjectPtr<UStaticMeshComponent> GlowPlane;
	
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
