// WeatherType.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WeatherType.generated.h"

UENUM(BlueprintType)
enum class EWeatherType : uint8
{
	Sunny UMETA(DisplayName = "Sunny"),
	Snowy UMETA(DisplayName = "Snowy"),
	Rainy UMETA(DisplayName = "Rainy"),

};