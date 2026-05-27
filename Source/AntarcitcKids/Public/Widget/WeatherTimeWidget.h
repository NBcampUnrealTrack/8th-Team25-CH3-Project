#pragma once
 
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Subsystem/TimeSubsystem.h"
#include "WeatherTimeWidget.generated.h"
 
// 전방선언 - RadialSlider.h include 없이 사용 (헤더 경로 이슈 우회)
class URadialSlider;
class UTextBlock;
class UButton;

UCLASS()
class ANTARCITCKIDS_API UWeatherTimeWidget : public UUserWidget
{
	GENERATED_BODY()
 
public:
	// 날씨 프리셋 - 실제 구현은 BP에서 오버라이드
	UFUNCTION(BlueprintImplementableEvent, Category = "Weather")
	void OnWeatherPresetChanged(int32 PresetIndex);
 
protected:
	// -----------------------------------------------------------------------
	// BindWidget : 변수명 = UMG 위젯 이름
	// -----------------------------------------------------------------------
 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URadialSlider> Time;
 
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TimeText;
 
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PeriodText;
 
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UButton> WeatherBtn0;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UButton> WeatherBtn1;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UButton> WeatherBtn2;
	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UButton> WeatherBtn3;
 
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
 
private:
	UFUNCTION()
	void HandleTimeChanged(double Pitch, FTimeOfDay TimeData);
 
	UFUNCTION()
	void HandleSliderValueChanged(float Value);
 
	UFUNCTION() void HandleSliderMouseCaptureBegin();
	UFUNCTION() void HandleSliderMouseCaptureEnd();
 
	UFUNCTION() void HandleWeatherBtn0Clicked();
	UFUNCTION() void HandleWeatherBtn1Clicked();
	UFUNCTION() void HandleWeatherBtn2Clicked();
	UFUNCTION() void HandleWeatherBtn3Clicked();
 
	void BindWeatherButtons();
	void UpdateVisuals(const FTimeOfDay& TimeData);
	static FString PeriodToString(ETimeOfDay Period);
 
	bool bUserDragging = false;
};