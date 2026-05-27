// WeatherSubsystem.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Actor/WeatherBase.h"

#include "Subsystem/WeatherSubsystem.h"

// Sets default values
AWeatherBase::AWeatherBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWeatherBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeatherBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeatherBase::NotifyEndWeatherFinished()
{
	if (UWorld* World = GetWorld())
	{
		if (UWeatherSubsystem* WeatherSubsystem = World->GetSubsystem<UWeatherSubsystem>())
		{
			WeatherSubsystem->OnCurrentWeatherEndFinished();
		}
	}
}