// QuestSubSystem.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Quest/QuestsTypes.h"
#include "Actor/TriggerMissionBase.h"
#include "QuestSubSystem.generated.h"


class ACityVehiclePawn; //HUD연결용
class UQuestBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuestListChange);


UCLASS()
class ANTARCITCKIDS_API UQuestSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// HUD 연결용
	UFUNCTION(BlueprintCallable)
	
	
	FString GenerateQuestID(UQuestBase* Quest);
	
	//서브 시스템의 UI 변화
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestListChange OnQuestListChange;
	
	//Quest를 QuestList에 등록, 제거하는 함수들
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void OnRegisterQuest( UQuestBase* RegisterQuest);
	void OnQuestsCompleted( UQuestBase* CompleteQuest);
	void OnQuestAborted();
	
	
	//퀘스트 리스트 맵 언로드 시의 초기화
	void OnPreLoadMap(const FString& MapName);

	
	//디버그용 함수
	void DebugQuestStatus();
	
	//UI에게 정보를 전달해 주기 위한 인터페이스 함수
	UFUNCTION(BlueprintCallable)
	TArray<UQuestBase*> GetQuestList() { return QuestsList;}
	UFUNCTION(BlueprintCallable)
	TArray<UQuestBase*> GetCompletedQuestList() { return CompletedQuestsList;}
	UFUNCTION(BlueprintCallable)
	TMap<EQuestClass, FQuestStats> GetQuestStatus() { return QuestStatus;}
	
private:

	// HUD 연결용
	UPROPERTY()
	TWeakObjectPtr<ACityVehiclePawn> CachedVehiclePawn;
	

	
	TArray<UQuestBase*> QuestsList;
	TArray<UQuestBase*> CompletedQuestsList;
	UDataTable* StartMissionList;
	TMap<EQuestClass, FQuestStats> QuestStatus;
	//데이터 로드된 데이터만 들고 있을 수 있도록 변경
	//EQuestClass : 퀘스트 종류
	//FQuestStats : 시도 횟수, 성공 횟수
	
	
};
