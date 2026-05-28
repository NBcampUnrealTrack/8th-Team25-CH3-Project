// WeatherTimeWidget.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Widget/WeatherTimeWidget.h"
#include "Components/Image.h"
#include "Components/RadialSlider.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Materials/MaterialInstanceDynamic.h"
 
// ============================================================================
// NativeConstruct / NativeDestruct
// ============================================================================
 
void UWeatherTimeWidget::NativeConstruct()
{
	Super::NativeConstruct();
 
	// TimeSubsystem 델리게이트 바인딩 + 초기값 설정
	if (UTimeSubsystem* TimeSys = GetWorld()->GetSubsystem<UTimeSubsystem>())
	{
		TimeSys->TimeChanged.AddDynamic(this, &UWeatherTimeWidget::HandleTimeChanged);
 
		if (Time)
		{
			Time->SetValue(TimeSys->GetCurrentTimeSeconds() / 86400.f);
		}
		
		if (Timer)
		{
			TimerMID = Timer->GetDynamicMaterial();
		
			if (TimerMID)
			{
				TimerMID->SetScalarParameterValue(
					TEXT("Progress"),
					TimeSys->GetCurrentTimeSeconds() / 86400.f
					);
			}
		}
		UpdateVisuals(TimeSys->GetCurrentTimeData());
	}
	
	// 날씨 버튼 바인딩
	BindWeatherButtons();
}
 
void UWeatherTimeWidget::NativeDestruct()
{
	// 위젯 파괴 시 델리게이트 해제 - dangling 바인딩 방지
	if (UWorld* World = GetWorld())
	{
		if (UTimeSubsystem* TimeSys = World->GetSubsystem<UTimeSubsystem>())
		{
			TimeSys->TimeChanged.RemoveDynamic(this, &UWeatherTimeWidget::HandleTimeChanged);
		}
	}
 
	Super::NativeDestruct();
}
 
// ============================================================================
// TimeSubsystem → 위젯 방향 (Tick에서 호출됨)
// ============================================================================
 
void UWeatherTimeWidget::HandleTimeChanged(double Pitch, FTimeOfDay TimeData)
{
	// 유저가 드래그 중이면 슬라이더 값 갱신 스킵 → 피드백 루프 방지
	if (!bUserDragging && Timer)
	{
		// Pitch(0~360) → 슬라이더(0~1)
		TimerMID->SetScalarParameterValue(
				TEXT("Progress"),
				static_cast<float>(Pitch) / 360.f
				);
	}
 
	UpdateVisuals(TimeData);
}
 
// ============================================================================
// 위젯 → TimeSubsystem 방향 (유저 조작)
// ============================================================================
 
void UWeatherTimeWidget::HandleSliderValueChanged(float Value)
{
	if (UTimeSubsystem* TimeSys = GetWorld()->GetSubsystem<UTimeSubsystem>())
	{
		// 슬라이더(0~1) → 초(0~86400)
		TimeSys->SetTime(Value * 86400.f);
	}
}
 
void UWeatherTimeWidget::HandleSliderMouseCaptureBegin()
{
	bUserDragging = true;
}
 
void UWeatherTimeWidget::HandleSliderMouseCaptureEnd()
{
	bUserDragging = false;
}
 
// ============================================================================
// 날씨 버튼 (나중에 구현 - 지금은 BP로 인덱스만 넘김)
// ============================================================================
 
void UWeatherTimeWidget::BindWeatherButtons()
{
	if (WeatherBtn0) WeatherBtn0->OnClicked.AddDynamic(this, &UWeatherTimeWidget::HandleWeatherBtn0Clicked);
	if (WeatherBtn1) WeatherBtn1->OnClicked.AddDynamic(this, &UWeatherTimeWidget::HandleWeatherBtn1Clicked);
	if (WeatherBtn2) WeatherBtn2->OnClicked.AddDynamic(this, &UWeatherTimeWidget::HandleWeatherBtn2Clicked);
	if (WeatherBtn3) WeatherBtn3->OnClicked.AddDynamic(this, &UWeatherTimeWidget::HandleWeatherBtn3Clicked);
}
 
void UWeatherTimeWidget::HandleWeatherBtn0Clicked() { OnWeatherPresetChanged(0); }
void UWeatherTimeWidget::HandleWeatherBtn1Clicked() { OnWeatherPresetChanged(1); }
void UWeatherTimeWidget::HandleWeatherBtn2Clicked() { OnWeatherPresetChanged(2); }
void UWeatherTimeWidget::HandleWeatherBtn3Clicked() { OnWeatherPresetChanged(3); }
 
// ============================================================================
// 비주얼 업데이트
// ============================================================================
 
void UWeatherTimeWidget::UpdateVisuals(const FTimeOfDay& TimeData)
{
	if (TimeText)
	{
		TimeText->SetText(FText::FromString(
			FString::Printf(TEXT("%02d:%02d"), TimeData.Hour, TimeData.Minute)
		));
	}
 
	if (PeriodText)
	{
		PeriodText->SetText(FText::FromString(PeriodToString(TimeData.Period)));
	}
}
 
FString UWeatherTimeWidget::PeriodToString(ETimeOfDay Period)
{
	switch (Period)
	{
		case ETimeOfDay::Dawn:    return TEXT("Dawn");
		case ETimeOfDay::Day:     return TEXT("Day");
		case ETimeOfDay::Evening: return TEXT("Evening");
		case ETimeOfDay::Night:   return TEXT("Night");
		default:                  return TEXT("Unknown");
	}
}