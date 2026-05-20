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
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEmergencyDetected);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEmergencyCleared);
	
	UPROPERTY(BlueprintAssignable)
	FOnEmergencyDetected OnEmergencyDetected;
	
	UPROPERTY(BlueprintAssignable)
	FOnEmergencyCleared OnEmergencyCleared;
	
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

	
	
	virtual void BeginPlay() override; 
	virtual void Tick(float DeltaSeconds) override;
	
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Quest")
	float ImpactDistance;

	void MoveTo();

private:
	FTimerHandle MoveTimerHandle;
	
	bool bIsImpact;
	
	
	FVector StartLocation;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Quest", meta=(AllowPrivateAccess = true))
	FVector DestLocation;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Quest", meta=(AllowPrivateAccess = true))
	float MoveDistance;
	

};
