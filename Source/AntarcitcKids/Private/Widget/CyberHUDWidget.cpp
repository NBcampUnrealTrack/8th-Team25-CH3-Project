// CyberHUDWidget.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Widget/CyberHUDWidget.h"
#include "Components/TextBlock.h"
#include "Components/CheckBox.h"

void UCyberHUDWidget::OnMissionRegistered(int32 MissionIndex, FName QuestName)
{
	if (!MissionTexts.IsValidIndex(MissionIndex)) return;
	
	MissionTexts[MissionIndex]->SetText(FText::FromName(QuestName));
}

void UCyberHUDWidget::OnMissionCompleted(int32 MissionIndex, FName QuestName, bool bCompleted)
{
	if (!MissionTexts.IsValidIndex(MissionIndex)) return;
	
	MissionTexts[MissionIndex]->SetText(FText::FromName(QuestName));
	MissionCheckBoxes[MissionIndex]->SetIsChecked(bCompleted);
	
	FLinearColor Color = bCompleted 
	? FLinearColor(0.f,1.f,0.f,1.f) 
	: FLinearColor(1.f, 0.f, 0.f,1.f);
	
	MissionTexts[MissionIndex]->SetColorAndOpacity(FSlateColor(Color));
}

void UCyberHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	MissionTexts = { MissionText_1, MissionText_2, MissionText_3, MissionText_4 };
	MissionCheckBoxes = { MissionCheckBox_1, MissionCheckBox_2, MissionCheckBox_3, MissionCheckBox_4};
}

