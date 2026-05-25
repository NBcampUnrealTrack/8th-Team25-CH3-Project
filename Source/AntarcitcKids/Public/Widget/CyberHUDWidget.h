#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CyberHUDWidget.generated.h"

class UTextBlock;
class UCheckBox;

UCLASS()
class ANTARCITCKIDS_API UCyberHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void OnMissionRegistered(int32 MissionIndex, FName QuestName);
	void OnMissionCompleted(int32 MissionIndex, FName QuestName, bool bCompleted);
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> MissionText_1;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> MissionText_2;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> MissionText_3;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> MissionText_4;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCheckBox> MissionCheckBox_1;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCheckBox> MissionCheckBox_2;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCheckBox> MissionCheckBox_3;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCheckBox> MissionCheckBox_4;
	
	TArray<UTextBlock*> MissionTexts;
	TArray<UCheckBox*> MissionCheckBoxes;
	
protected:
	virtual void NativeOnInitialized() override;
	
};
