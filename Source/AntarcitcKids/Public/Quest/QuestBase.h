// QuestBase.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Quest/QuestsTypes.h"
#include "QuestBase.generated.h"

class UNiagaraSystem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted,  UQuestBase* )


UCLASS()
class ANTARCITCKIDS_API UQuestBase : public UObject
{
	GENERATED_BODY()
	
public:
	FOnQuestCompleted OnQuestCompleted;
	
	virtual void OnInitialized(const FQuestInfo& QuestInfo);
	virtual void OnProgress();
	virtual void OnCompletetd();
	virtual void OnPaused();
	virtual void OnSuccess();
	virtual void OnFailed();
	virtual void SetEffect(UNiagaraSystem* Effect);
	virtual void PlayEffect();
	virtual bool IsQuestEnd();
	
	FQuestCompletedEvent GetQuestCompletedEvent() { return QuestCompletedEvent;}
	
	FQuestInfo GetCurrentQuestInfo() { return CurrentQuestInfo; }
	
protected:
	FQuestInfo CurrentQuestInfo;
	FQuestCompletedEvent QuestCompletedEvent;
	
	UNiagaraSystem* SuccessEffect;
	
	
};
