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
	EQuestClass RQuestClass = RegisterQuest->GetCurrentQuestInfo().QuestType;
	QuestStatus.FindOrAdd(RQuestClass).AttemptCount +=1;
	QuestStatus.Find(RQuestClass)->QuestName = RegisterQuest->GetCurrentQuestInfo().QuestName;
	
	
}





void UQuestSubSystem::OnQuestsCompleted(UQuestBase* CompleteQuest)
{
	EQuestClass CQuestClass = CompleteQuest->GetCurrentQuestInfo().QuestType;
	QuestsList.Remove(CompleteQuest);
	CompletedQuestsList.Add(CompleteQuest);
	if (CompleteQuest->GetCurrentQuestInfo().QuestProgress == EQuestAchivmentType::Succeed)
	{
		QuestStatus.Find(CQuestClass)->SuccessCount++;
		OnQuestListChange.Broadcast(CQuestClass, *QuestStatus.Find(CQuestClass));
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


