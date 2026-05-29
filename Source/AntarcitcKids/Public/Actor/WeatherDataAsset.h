// WeatherDataAsset.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WeatherBase.h"
#include "WeatherType.h"
#include "Engine/DataAsset.h"
#include "Engine/AssetManager.h"
#include "WeatherDataAsset.generated.h"

UCLASS()
class ANTARCITCKIDS_API UWeatherDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("WeatherData"),GetFName());
	}

	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<EWeatherType, TSubclassOf<AWeatherBase>> WeatherMap;
	
};
