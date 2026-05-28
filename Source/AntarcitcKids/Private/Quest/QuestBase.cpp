// QuestBase.cp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Quest/QuestBase.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AnimNodes/AnimNode_RandomPlayer.h"
#include "Subsystem/QuestSubSystem.h"
#include "Kismet/GameplayStatics.h"


void UQuestBase::OnInitialized(const FQuestInfo& QuestInfo)
{
	
	
	CurrentQuestInfo.QuestType = QuestInfo.QuestType;
	CurrentQuestInfo.QuestID = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubSystem>()->GenerateQuestID(this);
	CurrentQuestInfo.QuestClass = QuestInfo.QuestClass;
	CurrentQuestInfo.QuestID = QuestInfo.QuestID;
	CurrentQuestInfo.QuestName = QuestInfo.QuestName;
	CurrentQuestInfo.QuestState = EQuestProgressType::Initialized;
	CurrentQuestInfo.QuestProgress = EQuestAchivmentType::InProgress;
	CurrentQuestInfo.FreqQuest = QuestInfo.FreqQuest;
	CurrentQuestInfo.QuestLocation = QuestInfo.QuestLocation;
	
	QuestCompletedEvent.QuestClass = CurrentQuestInfo.QuestType;
	QuestCompletedEvent.QuestID = QuestInfo.QuestID;
	QuestCompletedEvent.QuestProgress = QuestInfo.QuestProgress;
	if (UQuestSubSystem* QuestSubSystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubSystem>())
	{
		QuestSubSystem->OnRegisterQuest(this);
	}

}

void UQuestBase::OnProgress()
{
	CurrentQuestInfo.QuestState = EQuestProgressType::Progress;
}

void UQuestBase::OnCompletetd()
{
	CurrentQuestInfo.QuestState = EQuestProgressType::Completed;
	OnQuestCompleted.Broadcast(this);
}

void UQuestBase::OnPaused()
{
	CurrentQuestInfo.QuestState = EQuestProgressType::Paused;
}

void UQuestBase::OnSuccess()
{
	UE_LOG(LogTemp,Warning, TEXT("OnSuccess Activate"));
	PlayEffect();
	CurrentQuestInfo.QuestProgress = EQuestAchivmentType::Succeed;
	QuestCompletedEvent.QuestProgress = EQuestAchivmentType::Succeed;
	OnCompletetd();
}

void UQuestBase::OnFailed()
{
	CurrentQuestInfo.QuestProgress = EQuestAchivmentType::Failed;
	QuestCompletedEvent.QuestProgress = EQuestAchivmentType::Failed;
	OnCompletetd();
}

void UQuestBase::SetEffect(UNiagaraSystem* Effect)
{
	SuccessEffect= Effect;
}

void UQuestBase::PlayEffect()
{
	if (!SuccessEffect) return;
	UE_LOG(LogTemp,Warning, TEXT("PlayEffect Activate"));
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this,
		SuccessEffect,
		CurrentQuestInfo.QuestLocation
		);
	
	
	
}

bool UQuestBase::IsQuestEnd()
{
	if (CurrentQuestInfo.QuestProgress != EQuestAchivmentType::InProgress)
	{
		return false;
	}
	return true;
}

