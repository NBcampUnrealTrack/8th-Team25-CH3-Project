// DigitalTwinConfiguration.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "../Subsystem/QuestSubSystem.h"
#include "../Subsystem/SimControlSubsystem.h"
#include "Subsystem/TimeSubsystem.h"
#include "../Subsystem/WeatherSubsystem.h"
#include "../Quest/QuestsTypes.h"
#include "Actor/WeatherType.h"
#include "Engine/GameInstance.h"
#include "DigitalTwinConfiguration.generated.h"

class ULoadingWidget;

DECLARE_MULTICAST_DELEGATE_OneParam(FWeatherChange, EWeatherType)

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
	FWeatherChange WeatherChange;
	
	
	UFUNCTION(BlueprintCallable)
	void InitializeSetting(FDigitialTwinSetting IDTS);
	
	UFUNCTION(BlueprintCallable)
	FDigitialTwinSetting GetDigitialTwinSetting() {return DTS;}
	
	void UpdateDTS();
	
	//=========================== 로딩 ==========================
	virtual void Init() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Loading")
	TSubclassOf<ULoadingWidget> LoadingWidgetClass;
	
	void BeginLoadingScreen();
	void EndLoadingScreen(UWorld* InLoadedWorld);
	
private:
	FDigitialTwinSetting DTS;
	UTimeSubsystem* TimeSubsystem;
	UWeatherSubsystem* WeatherSubsystem;

	
};
