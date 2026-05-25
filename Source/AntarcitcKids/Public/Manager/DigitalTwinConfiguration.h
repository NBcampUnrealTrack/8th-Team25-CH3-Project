// DigitalTwinConfiguration.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "QuestSubSystem.h"
#include "SimControlSubsystem.h"
#include "Manager/TimeSubsystem.h"
#include "WeatherSubsystem.h"
#include "QuestsTypes.h"
#include "Actor/WeatherType.h"
#include "Engine/GameInstance.h"
#include "DigitalTwinConfiguration.generated.h"

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
	UFUNCTION(BlueprintCallable)
	void InitializeSetting(FDigitialTwinSetting IDTS);
	
	UFUNCTION(BlueprintCallable)
	FDigitialTwinSetting GetDigitialTwinSetting() {return DTS;}
	
	
private:
	FDigitialTwinSetting DTS;

	
};
