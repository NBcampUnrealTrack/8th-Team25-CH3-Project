// QuestSubSystem.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Subsystem/QuestSubSystem.h"
#include "Subsystem/QuestSubSystem.h"
#include "CityVehiclePawn.h"
#include "AnimNodes/AnimNode_RandomPlayer.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "Quest/QuestBase.h"


UQuestSubSystem::UQuestSubSystem()
{
	OnLoadQuest();
}

void UQuestSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FCoreUObjectDelegates::PreLoadMap.AddUObject(
		this, &UQuestSubSystem::OnPreLoadMap);
	
}

void UQuestSubSystem::SetVehiclePawn(ACityVehiclePawn* InPawn)
{
	/*CachedVehiclePawn = InPawn;
	
	for (int32 i = 0; i < QuestsList.Num(); i++)
	{
		FName QuestName = QuestsList[i]->GetCurrentQuestInfo().QuestName;
		CachedVehiclePawn->RegisterMission(i, QuestName);
	}*/
}

FString UQuestSubSystem::GenerateQuestID(UQuestBase* Quest)
{
	FString MapName = Quest->GetWorld()->GetMapName();
	FName QuestName = Quest->GetCurrentQuestInfo().QuestName;

	EQuestClass RQuestClass = Quest->GetCurrentQuestInfo().QuestType;
	FQuestStats& Stats = QuestStatus.FindOrAdd(RQuestClass);
	int32 AttemptCount = Stats.AttemptCount;
	
	FString UniqueIDString = FString::Printf(TEXT("%s_%s_%d"),*MapName,*QuestName.ToString(),AttemptCount);
	
	return UniqueIDString;
	
	
}

void UQuestSubSystem::OnRegisterQuest(UQuestBase* RegisterQuest)
{
	RegisterQuest->OnQuestCompleted.AddUObject(this,&UQuestSubSystem::OnQuestsCompleted);
	
	
	QuestsList.Add(RegisterQuest);
	EQuestClass RQuestClass = RegisterQuest->GetCurrentQuestInfo().QuestType;
	FString EnumString = UEnum::GetValueAsString(RQuestClass);
	UE_LOG(LogTemp, Warning, TEXT("퀘스트 만들어짐. Type: %s"),*EnumString);
	FQuestStats& Stats = QuestStatus.FindOrAdd(RQuestClass);
	Stats.AttemptCount += 1;
	
	for (const TPair<EQuestClass, FQuestStats>& Pair : QuestStatus)
	{
		FString KeyString = UEnum::GetValueAsString(Pair.Key);
		UE_LOG(LogTemp, Warning, TEXT("Key: %s | AttemptCount: %d | SuccessCount: %d"),
			*KeyString,
			Pair.Value.AttemptCount,
			Pair.Value.SuccessCount
		);
	}
	
	
}





void UQuestSubSystem::OnQuestsCompleted(UQuestBase* CompleteQuest)
{
	EQuestClass CQuestClass = CompleteQuest->GetCurrentQuestInfo().QuestType;
	QuestsList.Remove(CompleteQuest);
	CompletedQuestsList.Add(CompleteQuest);
	if (CompleteQuest->GetCurrentQuestInfo().QuestProgress == EQuestAchivmentType::Succeed)
	{
		QuestStatus.Find(CQuestClass)->SuccessCount++;
		for (const TPair<EQuestClass, FQuestStats>& Pair : QuestStatus)
		{
			FString KeyString = UEnum::GetValueAsString(Pair.Key);
			UE_LOG(LogTemp, Warning, TEXT("Key: %s | AttemptCount: %d | SuccessCount: %d"),
				*KeyString,
				Pair.Value.AttemptCount,
				Pair.Value.SuccessCount
			);
		}
		OnQuestListChange.Broadcast();
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

void UQuestSubSystem::OnPreLoadMap(const FString& MapName)
{
	QuestsList.Empty();
	CompletedQuestsList.Empty();
	QuestStatus.Empty();
}


void UQuestSubSystem::OnPostLoadMap(UWorld* LoadedWorld)
{
	
	if (!IsValid(LoadedWorld)) return;
	

	
}

void UQuestSubSystem::OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources)
{
	if (IsValid(World)) return;
	
	
	/*QuestsList.Empty();
	CompletedQuestsList.Empty();
	QuestStatus.Empty();*/
}



