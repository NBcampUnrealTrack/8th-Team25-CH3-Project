// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/QuestSubSystem.h"
#include "Manager/QuestSubSystem.h"

#include "AnimNodes/AnimNode_RandomPlayer.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "Manager/QuestBase.h"


UQuestSubSystem::UQuestSubSystem()
{
	OnLoadQuest();
}

void UQuestSubSystem::OnRegisterQuest(UQuestBase* RegisterQuest)
{
	RegisterQuest->OnQuestCompleted.AddUObject(this,&UQuestSubSystem::OnQuestsCompleted);
	UE_LOG(LogTemp, Warning, TEXT("퀘스트 만들어짐."));
	
	QuestsList.Add(RegisterQuest);
}





void UQuestSubSystem::OnQuestsCompleted(UQuestBase* CompletedQuest)
{
	QuestsList.Remove(CompletedQuest);
	CompletedQuestsList.Add(CompletedQuest);
	
	FQuestCompletedEvent QuestCompletedEvent = CompletedQuest->GetQuestCompletedEvent();
	OnQuestCompletedNotify.Broadcast(QuestCompletedEvent);
	
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
