
#include "Manager/LightManager.h"

#include "Engine/DirectionalLight.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/TimeSubsystem.h"
#include "Road/StreetLightSource.h"


ALightManager::ALightManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALightManager::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetSubsystem<UTimeSubsystem>()->TimeChanged.AddUObject(this, &ALightManager::UpdateLights);
}

void ALightManager::UpdateLights(double Pitch)
{
	const bool bIsNight = Pitch >= 0.f && Pitch <= 180.f;
	
	TArray<AActor*> FoundLights;
	
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		AStreetLightSource::StaticClass(),
		FoundLights);
	
	for (AActor* Actor : FoundLights)
	{
		AStreetLightSource* Light = Cast<AStreetLightSource>(Actor);
		
		if (Light)
		{
			Light->SetLightEnabled(bIsNight);
		}
	}
	
	
}


