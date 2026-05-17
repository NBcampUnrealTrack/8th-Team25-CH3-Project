
#include "Road/StreetLightSource.h"

#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"


AStreetLightSource::AStreetLightSource()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(Root);
	
	//빛이 아래를 비추도록 설정
	SpotLight->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
}

//에디터에서 수정 시 바로 반영되도록 OnConstruction()함수에서 작성
void AStreetLightSource::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	SpotLight->SetIntensity(SpotLightIntensity);
	SpotLight->SetAttenuationRadius(AttenuationRadius);
	SpotLight->SetLightColor(LightColor);
	SpotLight->SetVisibility(bUseSpotLight);
}

void AStreetLightSource::SetLightEnabled(bool bEnabled) const
{
	if (SpotLight)
	{
		SpotLight->SetVisibility(bEnabled);
	}
}
