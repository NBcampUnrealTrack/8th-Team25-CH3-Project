// WeatherSubsystem.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/WeatherType.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/StreamableManager.h"
#include "WeatherSubsystem.generated.h"

class AWeatherBase;

UCLASS()
class ANTARCITCKIDS_API UWeatherSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	/*virtual void OnLoadedFinished(FPrimaryAssetId LoadedId);*/
	UFUNCTION(BlueprintCallable)
	void SetWeather(EWeatherType WeatherType);
	
	//=========================================
	UFUNCTION(BlueprintCallable)
	void OnCurrentWeatherEndFinished();
	//===========================================
	
	void GetCurrentWeather();
	
	void OnWorldBeginPlay(UWorld& InWorld) override;
	
	class UWeatherDataAsset* WeatherDataAsset;
	
protected:
	//===========================================
	void SpawnWeather(EWeatherType WeatherType);
	//===========================================
	
	TSharedPtr<FStreamableHandle> LoadingHandle;
	TMap<EWeatherType, TSubclassOf<AWeatherBase>> LoadedWeatherMap;
	TPair<EWeatherType, AWeatherBase*> CurrentWeather;
	
	//==================================
	EWeatherType PendingWeatherType;
	bool bIsChangingWeather = false;
//========================================
	
};
