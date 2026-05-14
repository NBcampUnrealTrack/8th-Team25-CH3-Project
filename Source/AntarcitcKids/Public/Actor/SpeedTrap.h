// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/TriggerMissionBase.h"
#include "Manager/QuestBase.h"
#include "SpeedTrap.generated.h"

/**
 * 
 */
UCLASS()
class ANTARCITCKIDS_API ASpeedTrap : public ATriggerMissionBase
{
	GENERATED_BODY()
	
public:
	ASpeedTrap();
	
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Speed")
	float SpeedUpperLimit;
	
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bIsPersistence; //구간단속 여부 확인
	
	UFUNCTION()
	void SetPawnSpeed(float SpeedKMH);
	
protected:
	
	virtual void BeginPlay() override;
	
	float PawnSpeed;

	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;


	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;
	
	virtual void SetQuestInfo() override;
	virtual void PersistentSpeedCheck();	
	FTimerHandle PersistenceSpeedCheckTimer;
private:
	
};
