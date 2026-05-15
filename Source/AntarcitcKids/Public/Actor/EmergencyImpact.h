// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/TriggerMissionBase.h"
#include "EmergencyImpact.generated.h"

/**
 * 
 */
UCLASS()
class ANTARCITCKIDS_API AEmergencyImpact : public ATriggerMissionBase
{
	GENERATED_BODY()
	
	
	
public:
	AEmergencyImpact();
	
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
		int32 OtherBodyIndex);



	
	void CheckImpact(AActor* OtherActor);
	void StartCheckImpact(AActor* OtherActor);
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Quest")
	float ImpactDistance;
	
	FTimerHandle ImpactCheckTimer;
private:
	bool bIsImpact;
};
