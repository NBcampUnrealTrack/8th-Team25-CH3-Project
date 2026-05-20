
#include "Manager/LightManager.h"

#include "Engine/DirectionalLight.h"
#include "Kismet/GameplayStatics.h"
#include "Road/StreetLightSource.h"


ALightManager::ALightManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALightManager::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateLights();
}

void ALightManager::UpdateLights()
{
	UE_LOG(LogTemp, Warning, TEXT("UpdateLights Called"));
	
	if (!DirectionalLight)
	{
		UE_LOG(LogTemp, Error, TEXT("DirectionalLight is NULL"));
		return;
	}
	
	const FRotator SunRotation = DirectionalLight->GetActorRotation();
	
	UE_LOG(LogTemp, Warning, TEXT("Pitch=%f, Yaw=%f, Roll=%f"),
	SunRotation.Pitch,
	SunRotation.Yaw,
	SunRotation.Roll);

	const float Pitch = SunRotation.Pitch;

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


