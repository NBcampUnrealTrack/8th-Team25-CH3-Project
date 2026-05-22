// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/WeatherBase.h"

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

