
#include "Road/StreetLightSource.h"
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
	
	//런타임중 바뀔 수 있음
	SpotLight->SetMobility(EComponentMobility::Stationary);
	
	//성능 보호 차원에서 넣음. 생각보다 괜찮으면 다시 킬 예정
	SpotLight->CastShadows = false;
	
	SpotLight->SetVisibility(false);
}

void AStreetLightSource::SetLightEnabled(bool bEnabled) 
{
	if (SpotLight)
	{
		SpotLight->SetVisibility(bEnabled);
	}
}
