// QuestSubSystem.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Subsystem/QuestSubSystem.h"
#include "CityVehiclePawn.h"
#include "AnimNodes/AnimNode_RandomPlayer.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "Quest/QuestBase.h"



//초기화 시에 맵 언로드 델리게이트 함수 구독
void UQuestSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FCoreUObjectDelegates::PreLoadMap.AddUObject(
		this, &UQuestSubSystem::OnPreLoadMap);
	
}


//고유 번호 생성
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


//퀘스트 등록 요청하여 List에 등록
void UQuestSubSystem::OnRegisterQuest(UQuestBase* RegisterQuest)
{
	
	RegisterQuest->OnQuestCompleted.AddUObject(this,&UQuestSubSystem::OnQuestsCompleted);
	QuestsList.Add(RegisterQuest);
	
	
	EQuestClass RQuestClass = RegisterQuest->GetCurrentQuestInfo().QuestType;
	FString EnumString = UEnum::GetValueAsString(RQuestClass);
	UE_LOG(LogTemp, Warning, TEXT("퀘스트 만들어짐. Type: %s"),*EnumString);
	FQuestStats& Stats = QuestStatus.FindOrAdd(RQuestClass);
	Stats.AttemptCount += 1;
	
	DebugQuestStatus();
	
	
}




//퀘스트가 완료되었을 때, QuestList Update, QuestStats Update
void UQuestSubSystem::OnQuestsCompleted(UQuestBase* CompleteQuest)
{
	EQuestClass CQuestClass = CompleteQuest->GetCurrentQuestInfo().QuestType;
	QuestsList.Remove(CompleteQuest);
	CompletedQuestsList.Add(CompleteQuest);
	if (CompleteQuest->GetCurrentQuestInfo().QuestProgress == EQuestAchivmentType::Succeed)
	{
		QuestStatus.Find(CQuestClass)->SuccessCount++;
		OnQuestListChange.Broadcast();
		DebugQuestStatus();
	}
}

void UQuestSubSystem::OnQuestAborted()
{
}



void UQuestSubSystem::OnPreLoadMap(const FString& MapName)
{
	QuestsList.Empty();
	CompletedQuestsList.Empty();
	QuestStatus.Empty();
}



void UQuestSubSystem::DebugQuestStatus()
{
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





