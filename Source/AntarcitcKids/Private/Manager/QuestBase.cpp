// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/QuestBase.h"

#include "Manager/QuestSubSystem.h"


void UQuestBase::OnInitialized(FQuestInfo QuestInfo)
{
	

	CurrentQuestInfo.QuestClass = QuestInfo.QuestClass;
	CurrentQuestInfo.QuestID = QuestInfo.QuestID;
	CurrentQuestInfo.QuestName = QuestInfo.QuestName;
	CurrentQuestInfo.QuestState = EQuestProgressType::Initialized;
	CurrentQuestInfo.QuestProgress = EQuestAchivmentType::InProgress;
	CurrentQuestInfo.FreqQuest = QuestInfo.FreqQuest;
	
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
	CurrentQuestInfo.QuestProgress = EQuestAchivmentType::Succeed;
	OnCompletetd();
}

void UQuestBase::OnFailed()
{
	CurrentQuestInfo.QuestProgress = EQuestAchivmentType::Failed;
	OnCompletetd();
}

bool UQuestBase::bIsSuccess()
{
	return false;
}
