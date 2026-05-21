// WeatherSubsystem.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Manager/WeatherSubsystem.h"

#include "Actor/WeatherBase.h"
#include "Kismet/GameplayStatics.h"
#include "Actor/WeatherDataAsset.h"
#include "AnimNodes/AnimNode_RandomPlayer.h"
#include "Engine/AssetManager.h"

void UWeatherSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	//에셋 메니저를 가져온다.
	UAssetManager& AssetManager = UAssetManager::Get();
	
	TArray<FPrimaryAssetId> AssetIds;
	AssetManager.GetPrimaryAssetIdList(FPrimaryAssetType(TEXT("WeatherData")), AssetIds);
	
	if (AssetIds.IsEmpty())	
		return;
	
	LoadingHandle = AssetManager.LoadPrimaryAssets(
		AssetIds,
		TArray<FName>(),
		FStreamableDelegate::CreateUObject(this,&UWeatherSubsystem::OnLoadedFinished,AssetIds[0])
		);
	
}

void UWeatherSubsystem::OnLoadedFinished(FPrimaryAssetId LoadedId)
{
	WeatherDataAsset = Cast<UWeatherDataAsset>(UAssetManager::Get().GetPrimaryAssetObject(LoadedId));
	
	if (WeatherDataAsset)
	{
		LoadedWeatherMap = WeatherDataAsset->WeatherMap;
	}
}

void UWeatherSubsystem::SetWeather(EWeatherType WeatherType)
{
	TSubclassOf<AWeatherBase>* FoundClass = LoadedWeatherMap.Find(WeatherType);
	
	if (CurrentWeather.Value)
		CurrentWeather.Value->Destroy();
	
	if (!FoundClass) return;
		
		
	AWeatherBase* SpawnedActor = GetWorld()->SpawnActor<AWeatherBase>(*FoundClass);
	CurrentWeather = TPair<EWeatherType, AWeatherBase*>(WeatherType, SpawnedActor);
}

void UWeatherSubsystem::GetCurrentWeather()
{
	
	if (AActor* Actor = UGameplayStatics::GetActorOfClass(GetWorld(),AWeatherBase::StaticClass()))
	{
		if (AWeatherBase* WeatherActor = Cast<AWeatherBase>(Actor))
		{
			CurrentWeather = {WeatherActor->CurrentWeatherType,WeatherActor};
		}
		
	}
}
