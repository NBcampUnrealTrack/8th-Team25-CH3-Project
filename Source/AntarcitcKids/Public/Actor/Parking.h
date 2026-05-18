// Parking.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Actor/TriggerMissionBase.h"
#include "Parking.generated.h"

UCLASS()
class ANTARCITCKIDS_API AParking : public ATriggerMissionBase
{
	GENERATED_BODY()
	
public:
	AParking();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Quest")
	float LimitAngle;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnParkingZoneEntered, AParking*, Zone);

	UPROPERTY(BlueprintAssignable)
	FOnParkingZoneEntered OnParkingZoneEntered;
	
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
	
	virtual void OnAreaOverlap(
		UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult) override;
	
	virtual void SetQuestInfo() override;
	
	void CheckParking(AActor* Actor);
	void CheckTimeOver();
	void TimeOver();
	void StartParkingCheck(AActor* Actor);
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Quest")
	float TimeOverLimit;
	
protected:
	FTimerHandle IsCorrectParking;
	FTimerHandle IsTimeOver;
	
private:
	
	
};
