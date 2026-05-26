// DigitalTwinConfiguration.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Manager/DigitalTwinConfiguration.h"


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


