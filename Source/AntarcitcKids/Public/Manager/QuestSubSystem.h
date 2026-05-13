// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "QuestSubSystem.generated.h"

/**
 * 
 */

class UQuestBase;

UCLASS()
class ANTARCITCKIDS_API UQuestSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UQuestSubSystem();
	void OnRegisterQuest( UQuestBase* RegisterQuest);
	void OnQuestsCompleted( UQuestBase* RegisterQuest);
	void OnLoadQuest();
	
	TArray<UQuestBase*> GetQuestList() { return QuestsList;}
	TArray<UQuestBase*> GetCompletedQuestList() { return CompletedQuestsList;}
	
private:
	TArray<UQuestBase*> QuestsList;
	TArray<UQuestBase*> CompletedQuestsList;
	UDataTable* StartMissionList;
	
	
	
};
