// SpeedTrap.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Actor/TriggerMissionBase.h"
#include "Manager/QuestBase.h"
#include "SpeedTrap.generated.h"

class UBoxComponent;

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
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Collision")
	UBoxComponent* EndGate;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpeedLimitEntered, float, SpeedLimit);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpeedLimitExited);
	
	UPROPERTY(BlueprintAssignable)
	FOnSpeedLimitEntered OnSpeedLimitEntered;
	
	UPROPERTY(BlueprintAssignable)
	FOnSpeedLimitExited OnSpeedLimitExited;
	
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
	
	
	UFUNCTION()
	virtual void OnEndGateOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult);
	
	
	
	virtual void SetQuestInfo() override;
	virtual void PersistentSpeedCheck();	
	FTimerHandle PersistenceSpeedCheckTimer;
private:
	float StartSpeed;
	float EndSpeed;
	
};
