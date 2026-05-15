// EmergencyImpact.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/TriggerMissionBase.h"
#include "EmergencyImpact.generated.h"

class USphereComponent;

UCLASS()
class ANTARCITCKIDS_API AEmergencyImpact : public ATriggerMissionBase
{
	GENERATED_BODY()
	
	
	
public:
	AEmergencyImpact();
	
	UPROPERTY(VisibleAnywhere,Category = "Impact")
	USphereComponent* ImpactCollision;
	
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

	UFUNCTION()
	virtual void OnImpactOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult);

	

	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Quest")
	float ImpactDistance;

private:
	bool bIsImpact;
};
