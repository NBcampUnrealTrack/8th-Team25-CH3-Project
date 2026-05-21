// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/TimeSubsystem.h"

#include "Engine/DirectionalLight.h"
#include "Kismet/GameplayStatics.h"

void UTimeSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	TArray<AActor*> Results;
	UGameplayStatics::GetAllActorsOfClass(&InWorld,ADirectionalLight::StaticClass(),Results);
	for (AActor* Result: Results)
	{
		if (ADirectionalLight* TempLight = Cast<ADirectionalLight>(Result))
		{
			CachedDirectionalLight = TempLight;
		}
	}
}

void UTimeSubsystem::SetTime(float Time)
{
	if (!CachedDirectionalLight.IsValid()) return;
	
	double Pitch = Time*0.25 + 90;
	CachedDirectionalLight->AddActorLocalRotation(FRotator(0,Pitch,0));
	TimeChanged.Broadcast(Pitch);
	
	
	
}
