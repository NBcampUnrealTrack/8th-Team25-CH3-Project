// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/TriggerMissionBase.h"
#include "Parking.generated.h"

/**
 * 
 */
UCLASS()
class ANTARCITCKIDS_API AParking : public ATriggerMissionBase
{
	GENERATED_BODY()
	
public:
	AParking();
	


	
protected:
	
	virtual void BeginPlay() override;
	

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
	
	void CheckParking(AActor* Actor);
	void StartParkingCheck(AActor* Actor);
	
protected:
	FTimerHandle IsCorrectParking;
	float LimitAngle;
};
