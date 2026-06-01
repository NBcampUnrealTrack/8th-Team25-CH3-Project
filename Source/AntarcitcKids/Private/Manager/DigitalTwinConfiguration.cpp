// DigitalTwinConfiguration.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Manager/DigitalTwinConfiguration.h"
#include "Subsystem/TimeSubsystem.h"
#include "Subsystem/WeatherSubsystem.h"
#include "Subsystem/SoundManagerSubsystem.h"
#include "MoviePlayer.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

// MoviePlayer란?
// Open Level 호출 시 렌더 스레드를 MoviePlayer가 가로채서 위젯을 그림
// 참고: 싱글플레이 전용, GetFirstLocalPlayerController()가 로딩 중 null일 수 있어서 타이밍 주의 필요

void UDigitalTwinConfiguration::Init()
{
	Super::Init();
	
	// MoviePlayer가 로딩 화면을 준비하기 직전 호출되는 델리게이트
	GetMoviePlayer()->OnPrepareLoadingScreen()
		.AddUObject(this, &UDigitalTwinConfiguration::BeginLoadingScreen);
	
	// 새 맵이 완전히 초기화된 뒤 엔진이 브로드캐스트하는 정적 델리게이트
	FCoreUObjectDelegates::PostLoadMapWithWorld
		.AddUObject(this, &UDigitalTwinConfiguration::EndLoadingScreen);
}

void UDigitalTwinConfiguration::InitializeSetting(FDigitialTwinSetting IDTS)
{
	DTS = IDTS;
	
	TimeSubsystem = GetWorld()->GetSubsystem<UTimeSubsystem>();
	WeatherSubsystem = GetWorld()->GetSubsystem<UWeatherSubsystem>();
}

void UDigitalTwinConfiguration::UpdateDTS()
{
	if (!TimeSubsystem || !WeatherSubsystem) return;
	
	DTS.CurrentTime= TimeSubsystem->GetCurrentTimeSeconds();
	DTS.CurrentWeatherType = WeatherSubsystem->GetCurrentWeather();
}

void UDigitalTwinConfiguration::OpenLevelWithLoading(FName LevelName)
{
	// 레벨 전환 전 사운드 음소거
	SoundManagerSubsystem = GetSubsystem<USoundManagerSubsystem>();
	if (SoundManagerSubsystem)
	{
		SoundManagerSubsystem->SetMasterVolume(0.0f);
	}
	
	// 로딩 위젯을 표시하도록 플래그 ON
	bShouldShowLoading = true;
	UGameplayStatics::OpenLevel(this, LevelName);
}

void UDigitalTwinConfiguration::NotifyLevelReady()
{
	// Main 레벨의 게임모드에서 BeginPlay 끝난 뒤 이 함수를 부름
	GetMoviePlayer()->StopMovie();
	
	// 로딩 화면 끝나면 볼륨 복구
	SoundManagerSubsystem = GetSubsystem<USoundManagerSubsystem>();
	if (SoundManagerSubsystem)
	{
		SoundManagerSubsystem->SetMasterVolume(1.0f);
	}
}

void UDigitalTwinConfiguration::BeginLoadingScreen()
{
	if (!bShouldShowLoading) return;
	bShouldShowLoading = false;
	
	FLoadingScreenAttributes LoadingScreen;
	
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = false; // 로딩 화면 끝내는 타이밍을 직접 정하기 위해
	
	if (LoadingWidgetClass)
	{
		UUserWidget* LoadingWidgetInstance = CreateWidget<UUserWidget>(GetFirstLocalPlayerController(), LoadingWidgetClass);
		if (LoadingWidgetInstance)
		{
			// MoviePlayer는 Slate 레이어에서 동작
			// TakeWidget() : UMG 위젯 → Slate 위젯(SWidget) 변환
			LoadingScreen.WidgetLoadingScreen = LoadingWidgetInstance->TakeWidget();
		}
	}

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

void UDigitalTwinConfiguration::EndLoadingScreen(UWorld* InLoadedWorld)
{
	// 현재는 NotifyLevelReady()에서 처리하므로 비워둬도 됨
	// 나중에 후처리 로직 추가할 때 사용
}
