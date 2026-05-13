// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Manager/QuestBase.h"
#include "TrafficLightQuest.generated.h"

/**
 * 
 */
UCLASS()
class ANTARCITCKIDS_API UTrafficLightQuest : public UQuestBase
{
	GENERATED_BODY()
	
public:
	UTrafficLightQuest();
	
	virtual bool bIsSuccess() override;
	

	
private:

	
};
