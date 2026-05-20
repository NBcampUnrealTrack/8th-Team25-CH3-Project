// QuestSubSystem.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Manager/QuestSubSystem.h"
#include "Manager/QuestSubSystem.h"
#include "CityVehiclePawn.h"
#include "AnimNodes/AnimNode_RandomPlayer.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "Manager/QuestBase.h"


UQuestSubSystem::UQuestSubSystem()
{
	OnLoadQuest();
}

void UQuestSubSystem::SetVehiclePawn(ACityVehiclePawn* InPawn)
{
	CachedVehiclePawn = InPawn;
	
	for (int32 i = 0; i < QuestsList.Num(); i++)
	{
		FName QuestName = QuestsList[i]->GetCurrentQuestInfo().QuestName;
		CachedVehiclePawn->RegisterMission(i, QuestName);
	}
}

void UQuestSubSystem::OnRegisterQuest(UQuestBase* RegisterQuest)
{
	RegisterQuest->OnQuestCompleted.AddUObject(this,&UQuestSubSystem::OnQuestsCompleted);
	UE_LOG(LogTemp, Warning, TEXT("퀘스트 만들어짐."));
	
	QuestsList.Add(RegisterQuest);
	
}





void UQuestSubSystem::OnQuestsCompleted(FQuestCompletedEvent& QuestCompletedEvent)
{
	QuestListChangeStruct.QuestTypeList.FindOrAdd(QuestCompletedEvent.QuestClass) += 1;
	FQuestStats& Stats = QuestListChangeStruct.QuestTypeStats.FindOrAdd(QuestCompletedEvent.QuestClass);
	Stats.AttemptCount += 1;
	if (QuestCompletedEvent.QuestProgress == EQuestAchivmentType::Succeed)
	{
		QuestListChangeStruct.QuestTypeStats[QuestCompletedEvent.QuestClass].SuccessCount +=1;
	}
	
	OnQuestListChange.Broadcast(QuestListChangeStruct);
	
	if (CachedVehiclePawn.IsValid())
	{
		// QuestID(FName)를 미션 인덱스로 쓰기 위해 변환
		// DataTable의 RowName이 곧 QuestID이므로 인덱스 기반으로 매핑 필요
		int32 MissionIndex = QuestsList.IndexOfByPredicate([&](UQuestBase* Q){
			return Q->GetQuestCompletedEvent().QuestID == QuestCompletedEvent.QuestID;
		});
        
		if (MissionIndex != INDEX_NONE)
		{
			CachedVehiclePawn->CompleteMission(MissionIndex, QuestCompletedEvent.QuestName);
		}
	}
	
}

void UQuestSubSystem::OnQuestAborted()
{
}

void UQuestSubSystem::OnLoadQuest()
{
	if (!StartMissionList) return;
	TArray<FQuestInfo*> Rows;
	StartMissionList->GetAllRows<FQuestInfo>(TEXT(""), Rows);
	for (auto& row : Rows)
	{
		
		if (row->QuestClass)
		{
			UQuestBase* NewQuest = NewObject<UQuestBase>(this,row->QuestClass);
			NewQuest->OnInitialized(*row);
			OnRegisterQuest(NewQuest);
		}
	}
}


