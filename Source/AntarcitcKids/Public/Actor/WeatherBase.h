// WeatherBase.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeatherType.h"
#include "WeatherBase.generated.h"

UCLASS()
class ANTARCITCKIDS_API AWeatherBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeatherBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Weather")
	EWeatherType CurrentWeatherType;

	//===============================================================================
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Weather")
	void RequestEndWeather();

	UFUNCTION(BlueprintCallable, Category="Weather")
	void NotifyEndWeatherFinished();
	//===============================================================================
	
};
