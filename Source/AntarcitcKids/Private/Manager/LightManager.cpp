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
	GetWorld()->GetSubsystem<UTimeSubsystem>()->TimeChanged.AddDynamic(
		this, &ALightManager::UpdateLights);
}

void ALightManager::UpdateLights(double Pitch, FTimeOfDay TimeData)
{
	const bool bIsNight = (TimeData.Period == ETimeOfDay::Night ||
						   TimeData.Period == ETimeOfDay::Dawn);

	TArray<AActor*> FoundLights;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStreetLightSource::StaticClass(), FoundLights);

	for (AActor* Actor : FoundLights)
	{
		if (AStreetLightSource* Light = Cast<AStreetLightSource>(Actor))
			Light->SetLightEnabled(bIsNight);
	}
}

