// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/QuestCameraWidget.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h"




void UQuestCameraWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (IsValid(CameraImage) && IsValid(MissionCamBD))
	{
		//CameraImage->SetVisibility(ESlateVisibility::Collapsed);
		MissionCamBD->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UQuestCameraWidget::SetRenderTarget(UTextureRenderTarget2D* InRenderTarget)
{
	if (!InRenderTarget || !CameraImage)
		return;

	CameraImage->SetBrushResourceObject(InRenderTarget);
}


void UQuestCameraWidget::ToggleCameraView()
{
	if (!CameraImage || !MissionCamBD)
		return;
	const ESlateVisibility NewVis = IsCameraViewVisible()
	? ESlateVisibility::Collapsed 
	: ESlateVisibility::SelfHitTestInvisible;
	
	//CameraImage->SetVisibility(NewVis);
	MissionCamBD->SetVisibility(NewVis);
}

void UQuestCameraWidget::TurnOnCameraView()
{
	if (!CameraImage || !MissionCamBD)
		return;
	const ESlateVisibility NewVis = ESlateVisibility::SelfHitTestInvisible;
	//CameraImage->SetVisibility(NewVis);
	MissionCamBD->SetVisibility(NewVis);
}

void UQuestCameraWidget::TurnOffCameraView()
{
	if (!CameraImage || !MissionCamBD)
		return;
	const ESlateVisibility NewVis = ESlateVisibility::Collapsed;
	//CameraImage->SetVisibility(NewVis);
	MissionCamBD->SetVisibility(NewVis);
}

bool UQuestCameraWidget::IsCameraViewVisible() const
{
	//return CameraImage && CameraImage->GetVisibility() != ESlateVisibility::Collapsed;
	return CameraImage && MissionCamBD && MissionCamBD->GetVisibility() != ESlateVisibility::Collapsed;
}
