// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/TriggerMissionBase.h"
#include "RoadSpeedZone.generated.h"

UCLASS()
class ANTARCITCKIDS_API ARoadSpeedZone : public ATriggerMissionBase
{
	GENERATED_BODY()
	
public:
	ARoadSpeedZone();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Road")
	float NormalSpeedKMH;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNormalSpeedChanged, float, NormalSpeedKMH);
	
	UPROPERTY(BlueprintAssignable)
	FOnNormalSpeedChanged OnNormalSpeedChanged;
	
protected:
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	// 이탈해도 속도 유지 되기 때문에 EndOverlap은 필요 없음
};
