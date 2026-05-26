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
	UE_LOG(LogTemp,Warning, TEXT("Initialize 발동"));
	//에셋 메니저를 가져온다.
	UAssetManager& AssetManager = UAssetManager::Get();
	
	TArray<FPrimaryAssetId> AssetIds;
	AssetManager.GetPrimaryAssetIdList(FPrimaryAssetType(TEXT("WeatherData")), AssetIds);
	
	if (AssetIds.IsEmpty())
	{
		UE_LOG(LogTemp,Warning, TEXT("AssetId.IsEmpty() 발동"));
		return;
	}
		
	
	
	LoadingHandle = AssetManager.LoadPrimaryAssets(
		AssetIds,
		TArray<FName>()
		);
	//FStreamableDelegate::CreateUObject(this,&UWeatherSubsystem::OnLoadedFinished,AssetIds[0]
	if (LoadingHandle.IsValid())
	{
		LoadingHandle->WaitUntilComplete();
	}
	
	for (const FPrimaryAssetId& Id : AssetIds )
	{
		UWeatherDataAsset* DataAsset = Cast<UWeatherDataAsset>(UAssetManager::Get().GetPrimaryAssetObject(Id));
		if (DataAsset)
		{
			LoadedWeatherMap.Append(DataAsset->WeatherMap);
			
		}
	}
	
}

//비동기 로딩을 위해 만들어졌으나, 비동기 로딩 시에 시작 시 나오지 않는 현상 발견

/*void UWeatherSubsystem::OnLoadedFinished(FPrimaryAssetId LoadedId)
{
	WeatherDataAsset = Cast<UWeatherDataAsset>(UAssetManager::Get().GetPrimaryAssetObject(LoadedId));
	
	if (WeatherDataAsset)
	{
		LoadedWeatherMap = WeatherDataAsset->WeatherMap;
		UE_LOG(LogTemp,Warning, TEXT("OnLoadedFinish 발동"));
		GetCurrentWeather();
	}
}*/

void UWeatherSubsystem::SetWeather(EWeatherType WeatherType)
{
	if (WeatherType == CurrentWeather.Key)
	{
		UE_LOG(LogTemp,Warning, TEXT("같은 날씨로는 변경 불가능"));
		return;
	}
	
	//===================================================================
	PendingWeatherType = WeatherType;

	if (bIsChangingWeather)
	{
		UE_LOG(LogTemp, Warning, TEXT("날씨 전환 중: 요청만 저장"));
		return;
	}
	
	if (IsValid(CurrentWeather.Value))
	{
		bIsChangingWeather = true;
		UE_LOG(LogTemp, Warning, TEXT("현재 날씨 종료 요청"));
		CurrentWeather.Value->RequestEndWeather();
		return;
	}
	//===================================================================

	/*
	TSubclassOf<AWeatherBase>* FoundClass = LoadedWeatherMap.Find(WeatherType);
	UE_LOG(LogTemp,Warning, TEXT("SetWeather 발동"));
	/*UE_LOG(LogTemp,Warning, TEXT("CurrentWeather: {%s, %s}"), *UEnum::GetValueAsString(CurrentWeather.Key),*CurrentWeather.Value->GetFName().ToString());*/
	/*
	if (IsValid(CurrentWeather.Value))
	{
		CurrentWeather.Value->Destroy();
		UE_LOG(LogTemp,Warning, TEXT("CurrentWeather 삭제"));
	}
	
	
	
	if (!FoundClass) return;
		
		
	AWeatherBase* SpawnedActor = GetWorld()->SpawnActor<AWeatherBase>(*FoundClass);
	CurrentWeather = TPair<EWeatherType, AWeatherBase*>(WeatherType, SpawnedActor);
	/*UE_LOG(LogTemp,Warning, TEXT("CurrentWeather: {%s, %s}"), *UEnum::GetValueAsString(CurrentWeather.Key),*CurrentWeather.Value->GetFName().ToString());*/
	
	
	SpawnWeather(WeatherType);
}


EWeatherType UWeatherSubsystem::GetCurrentWeather()
{
	if (AActor* Actor = UGameplayStatics::GetActorOfClass(GetWorld(),AWeatherBase::StaticClass()))
	{
		if (AWeatherBase* WeatherActor = Cast<AWeatherBase>(Actor))
		{
			CurrentWeather = {WeatherActor->CurrentWeatherType,WeatherActor};
		}
		else
		{
			UE_LOG(LogTemp,Warning, TEXT("WeatherActor 아님 발동"));
		}
		
	}
	else
	{
		/*UE_LOG(LogTemp,Warning, TEXT("탐색 불가"));*/
	}
	return CurrentWeather.Key;
}

void UWeatherSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	GetCurrentWeather();
;	
	
}

//=========================================================================================
void UWeatherSubsystem::SpawnWeather(EWeatherType WeatherType)
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnWeather 요청 타입: %s"), *UEnum::GetValueAsString(WeatherType));
	
	TSubclassOf<AWeatherBase>* FoundClass = LoadedWeatherMap.Find(WeatherType);

	UE_LOG(LogTemp, Warning, TEXT("SpawnWeather 발동"));

	if (!FoundClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("날씨 클래스 찾기 실패"));
		bIsChangingWeather = false;
		return;
	}

	AWeatherBase* SpawnedActor = GetWorld()->SpawnActor<AWeatherBase>(*FoundClass);

	CurrentWeather = TPair<EWeatherType, AWeatherBase*>(WeatherType, SpawnedActor);
	bIsChangingWeather = false;
}


void UWeatherSubsystem::OnCurrentWeatherEndFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("현재 날씨 종료 완료"));

	if (IsValid(CurrentWeather.Value))
	{
		CurrentWeather.Value->Destroy();
		UE_LOG(LogTemp, Warning, TEXT("기존 날씨 Destroy"));
	}

	CurrentWeather.Value = nullptr;

	SpawnWeather(PendingWeatherType);
}

//현제 날씨 로직이 무사히 끝날 수 있도록 기다려주는 함수, 그 후 디스트로이로 액터를 부숨
//==================================================================================================