// DigitalTwinConfiguration.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/WeatherType.h"
#include "Engine/GameInstance.h"
#include "DigitalTwinConfiguration.generated.h"

class ULoadingWidget;
class UTimeSubsystem;
class UWeatherSubsystem;
class USoundManagerSubsystem;

// 날씨 변경 이벤트 외부에 알리기
DECLARE_MULTICAST_DELEGATE_OneParam(FWeatherChange, EWeatherType)

// 시뮬레이션 설정값 구조체
USTRUCT(BlueprintType)
struct FDigitialTwinSetting
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float CurrentTime;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EWeatherType CurrentWeatherType;
};

UCLASS()
class ANTARCITCKIDS_API UDigitalTwinConfiguration : public UGameInstance
{
	GENERATED_BODY()
	
public:
	
	// MoviePlayer 델리게이트 등록
	virtual void Init() override;
	
	UFUNCTION(BlueprintCallable)
	void InitializeSetting(FDigitialTwinSetting IDTS);
	
	UFUNCTION(BlueprintCallable)
	FDigitialTwinSetting GetDigitialTwinSetting() {return DTS;}
	
	void UpdateDTS();
	
	FWeatherChange WeatherChange;
	
	// 로딩
	// 로딩 위젯을 띄우면서 레벨을 여는 함수 (일반 Open Level 대신 이 함수를 사용해야 로딩창이 표시됨)
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void OpenLevelWithLoading(FName LevelName);
	
	UPROPERTY(EditDefaultsOnly, Category = "Loading")
	TSubclassOf<UUserWidget> LoadingWidgetClass;
	
	UFUNCTION(BlueprintCallable)
	void NotifyLevelReady();
	
private:
	
	void BeginLoadingScreen();
	void EndLoadingScreen(UWorld* InLoadedWorld);
	
	FDigitialTwinSetting DTS;
	
	UPROPERTY()
	TObjectPtr<UTimeSubsystem> TimeSubsystem = nullptr;
	
	UPROPERTY()
	TObjectPtr<UWeatherSubsystem> WeatherSubsystem = nullptr;
	
	UPROPERTY()
	TObjectPtr<USoundManagerSubsystem> SoundManagerSubsystem = nullptr;
	
	// true일 때만 BeginLoadingScreen에서 위젯을 세팅
	// 의도하지 않은 레벨 전환에서 로딩창이 뜨는 것을 방지
	bool bShouldShowLoading = false;
};
