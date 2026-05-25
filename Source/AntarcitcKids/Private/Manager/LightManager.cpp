#include "Manager/LightManager.h"
#include "Engine/DirectionalLight.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/TimeSubsystem.h"
#include "Light/LightSourceBase.h"

ALightManager::ALightManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALightManager::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> FoundLights;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		ALightSourceBase::StaticClass(),
		FoundLights
	);

	for (AActor* Actor : FoundLights)
	{
		if (ALightSourceBase* Light = Cast<ALightSourceBase>(Actor))
		{
			ManagedLights.Add(Light);
		}
	}

	if (UTimeSubsystem* TimeSubsystem = GetWorld()->GetSubsystem<UTimeSubsystem>())
	{
		TimeSubsystem->TimeChanged.AddDynamic(
			this,
			&ALightManager::UpdateLights
		);
	}
}

void ALightManager::UpdateLights(double Pitch, FTimeOfDay TimeData)
{
	const bool bIsNight = (TimeData.Period == ETimeOfDay::Night ||
						   TimeData.Period == ETimeOfDay::Dawn);

	for (ALightSourceBase* Light : ManagedLights)
	{
		if (Light)
		{
			Light->SetLightEnabled(bIsNight);
		}
	}
}

