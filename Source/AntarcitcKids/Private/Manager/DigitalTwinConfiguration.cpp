// DigitalTwinConfiguration.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Manager/DigitalTwinConfiguration.h"
#include "MoviePlayer.h"
#include "Blueprint/UserWidget.h"
#include "Widget/LoadingWidget.h"
#include "Widgets/SWeakWidget.h"


void UDigitalTwinConfiguration::InitializeSetting(FDigitialTwinSetting IDTS)
{
	DTS = IDTS;
	TimeSubsystem = GetWorld()->GetSubsystem<UTimeSubsystem>();
	WeatherSubsystem = GetWorld()->GetSubsystem<UWeatherSubsystem>();
	
	
}

void UDigitalTwinConfiguration::UpdateDTS()
{
	if (TimeSubsystem || WeatherSubsystem) return;
	
	DTS.CurrentTime= TimeSubsystem->GetCurrentTimeSeconds();
	DTS.CurrentWeatherType = WeatherSubsystem->GetCurrentWeather();
	
	
}

//=============================로딩==========================

void UDigitalTwinConfiguration::Init()
{
	Super::Init();
	
	GetMoviePlayer()->OnPrepareLoadingScreen().AddUObject(this, &UDigitalTwinConfiguration::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UDigitalTwinConfiguration::EndLoadingScreen);
}

void UDigitalTwinConfiguration::BeginLoadingScreen()
{
	if (bIsFirstLoad)
	{
		bIsFirstLoad = false;
		
		return;
	}
	
	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = true; // 맵 초기화 로딩 완료 Bool값
	// LoadingScreen.MinimumLoadingScreenDisplayTime = 1.0f; // 로딩 최소시간 재사용 가능성 낮음
	
	if (LoadingWidgetClass)
	{
		ULoadingWidget* LoadingWidgetInstance = CreateWidget<ULoadingWidget>(this, LoadingWidgetClass);
		if (LoadingWidgetInstance)
		{
			LoadingScreen.WidgetLoadingScreen = LoadingWidgetInstance->TakeWidget();
		}
	}

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

void UDigitalTwinConfiguration::EndLoadingScreen(UWorld* InLoadedWorld)
{
	// 로딩 완료 후 처리 이건 과제에선 사용 X
}

