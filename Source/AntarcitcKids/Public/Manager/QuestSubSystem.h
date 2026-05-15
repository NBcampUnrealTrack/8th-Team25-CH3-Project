// QuestSubSystem.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Manager/QuestsTypes.h"
#include "QuestSubSystem.generated.h"


class UQuestBase;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnQuestListChange, FQuestListChange)


UCLASS()
class ANTARCITCKIDS_API UQuestSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	//서브 시스템의 UI 변화
	FOnQuestListChange OnQuestListChange;
	
	UQuestSubSystem();
	void OnRegisterQuest( UQuestBase* RegisterQuest);
	void OnQuestsCompleted( FQuestCompletedEvent& QuestCompletedEvent);
	void OnQuestAborted();
	void OnLoadQuest();
	
	TArray<UQuestBase*> GetQuestList() { return QuestsList;}
	TArray<UQuestBase*> GetCompletedQuestList() { return CompletedQuestsList;}
	
private:
	TArray<UQuestBase*> QuestsList;
	TArray<UQuestBase*> CompletedQuestsList;
	FQuestListChange QuestListChangeStruct;
	UDataTable* StartMissionList;
	//데이터 로드된 데이터만 들고 있을 수 있도록 변경
	
	
	
};
