#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SimPauseWidget.generated.h"

UCLASS()
class ANTARCITCKIDS_API USimPauseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// X 버튼 OnClicked에 연결
	UFUNCTION(BlueprintCallable, Category = "UI")
	void CloseWidget();

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};
