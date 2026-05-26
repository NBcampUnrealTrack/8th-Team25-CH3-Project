// QuestSubSystem.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Manager/QuestsTypes.h"
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
	void SetVehiclePawn(ACityVehiclePawn* InPawn);
	int32 SetQuestID() {return QuestID++;}
	
	FString GenerateQuestID(UQuestBase* Quest);
	
	//서브 시스템의 UI 변화
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestListChange OnQuestListChange;
	
	UQuestSubSystem();
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void OnRegisterQuest( UQuestBase* RegisterQuest);
	void OnQuestsCompleted( UQuestBase* CompleteQuest);
	void OnQuestAborted();
	void OnLoadQuest();
	
	void OnPreLoadMap(const FString& MapName);
	void OnPostLoadMap(UWorld* LoadedWorld);
	void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);

	
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
	
	
	
	int QuestID = 1;
	
};
