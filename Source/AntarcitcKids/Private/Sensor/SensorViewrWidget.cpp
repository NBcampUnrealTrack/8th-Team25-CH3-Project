// Copyright NBC, Inc. All Rights Reserved.

#include "Sensor/SensorViewrWidget.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"

void USensorViewrWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//센서 경계와 라이더 경계는 미리 숨겨놓는다.
	if (SensorBorder)
		SensorBorder->SetVisibility(ESlateVisibility::Collapsed);
	if (LidarBorder)
		LidarBorder->SetVisibility(ESlateVisibility::Collapsed);
}

//그림 연결
void USensorViewrWidget::SetRenderTarget(UTextureRenderTarget2D* InRenderTarget)
{
	if (!InRenderTarget || !SensorImage)
		return;
	
	SensorImage->SetBrushResourceObject(InRenderTarget);
}

void USensorViewrWidget::SetLidarRenderTarget(UTexture2D* InRenderTarget)
{
	if (!InRenderTarget || !LidarImage)
		return;
	
	LidarImage->SetBrushResourceObject(InRenderTarget);
}


void USensorViewrWidget::ToggleCameraView()
{
	if (!SensorBorder)
		return;
	//보여주고 있으면, 숨기고, 숨기고 있으면 보여준다.
	const ESlateVisibility NewVis = IsCameraViewVisible()
		? ESlateVisibility::Collapsed
		: ESlateVisibility::SelfHitTestInvisible;

	SensorBorder->SetVisibility(NewVis);
}

void USensorViewrWidget::ToggleLidarView()
{
	if (!LidarBorder)
		return;
	//보여주고 있으면, 숨기고, 숨기고 있으면 보여준다.
	const ESlateVisibility NewVis = IsLidarViewVisible()
		? ESlateVisibility::Collapsed
		: ESlateVisibility::SelfHitTestInvisible;

	LidarBorder->SetVisibility(NewVis);
}

bool USensorViewrWidget::IsCameraViewVisible() const
{
	return SensorBorder && SensorBorder->GetVisibility() != ESlateVisibility::Collapsed;
}

bool USensorViewrWidget::IsLidarViewVisible() const
{
	return LidarBorder && LidarBorder->GetVisibility() != ESlateVisibility::Collapsed;
}
