// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Manager/QuestsTypes.h"
#include "QuestBase.generated.h"

/**
 * 
 */
class UNiagaraSystem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted,  UQuestBase*)


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
	
	FQuestCompletedEvent GetQuestCompletedEvent() { return QuestCompletedEvent;}
	
protected:
	EQuestClass QuestClass;
	FQuestInfo CurrentQuestInfo;
	FQuestCompletedEvent QuestCompletedEvent;
	
	UNiagaraSystem* SuccessEffect;
	
	
};
