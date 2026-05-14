// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "QuestsTypes.generated.h" 

class UQuestBase;


UENUM(BlueprintType)
enum class EQuestProgressType : uint8
{
	Initialized UMETA(DisplayName = "Initialized"),
	Progress UMETA(DisplayName = "Progress"),
	Completed UMETA(DisplayName = "Completed"),
	Paused UMETA(DisplayName = "Paused"),
	Cancelled UMETA(DisplayName = "Cancelled"),
};

UENUM(BlueprintType)
enum class EQuestAchivmentType : uint8
{
	Succeed UMETA(DisplayName = "Succeed"),
	Failed UMETA(DisplayName = "Failed"),
	InProgress UMETA(DisplayName = "InProgress")

};

UENUM(BlueprintType)
enum class EQuestClass : uint8
{
	SpeedTrap UMETA(DisplayName = "SpeedTrap"),
	TrafficSignal UMETA(DisplayName = "TrafficSignal"),
	EmergencyStop UMETA(DisplayName = "EmergencyStop"),
	Parking UMETA(DisplayName = "Parking")

};

USTRUCT(BlueprintType)
struct FQuestCompletedEvent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Quest")
	EQuestClass QuestClass; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Quest")
	FName QuestID;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Quest")
	EQuestAchivmentType QuestProgress;
	
	
};

USTRUCT(BlueprintType)
struct FQuestInfo : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Quest")
	TSubclassOf<UQuestBase> QuestClass; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Quest")
	FName QuestID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Quest")
	FName QuestName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Quest")
	FString Description;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Quest")
	EQuestProgressType QuestState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Quest")
	EQuestAchivmentType QuestProgress;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Quest")
	TArray<FName> FreqQuest;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Quest")
	FVector QuestLocation;
	
	
	
};