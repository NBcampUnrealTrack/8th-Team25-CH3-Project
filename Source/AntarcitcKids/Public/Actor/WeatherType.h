// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeatherType.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EWeatherType : uint8
{
	Sunny UMETA(DisplayName = "Sunny"),
	Snowy UMETA(DisplayName = "Snowy"),
	Rainy UMETA(DisplayName = "Rainy"),

};