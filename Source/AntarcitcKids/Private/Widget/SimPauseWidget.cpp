// SimPauseWidget.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Widget/SimPauseWidget.h"
#include "Subsystem/SimControlSubsystem.h"
#include "Framework/Application/SlateApplication.h"

void USimPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true); // x 키 입력 받으려면 필요
}

void USimPauseWidget::CloseWidget()
{
	// 시뮬 재개
	if (UGameInstance* GI = GetGameInstance())
		if (USimControlSubsystem* Ctrl = GI->GetSubsystem<USimControlSubsystem>())
			Ctrl->Play();

	// 마우스 숨기고 게임 입력으로 복귀
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());
	}

	RemoveFromParent();
}

FReply USimPauseWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// ESC로도 닫기
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		CloseWidget();
		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
