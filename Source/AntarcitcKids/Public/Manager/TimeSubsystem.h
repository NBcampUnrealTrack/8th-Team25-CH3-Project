// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TimeSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FTimeChanged, double)

class ADirectionalLight;
/**
 * 
 */
UCLASS()
class ANTARCITCKIDS_API UTimeSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	

	
public:
	FTimeChanged TimeChanged;
	
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	UFUNCTION(BlueprintCallable)
	void SetTime(float Time);
	
	
private:
	
	float CurrentTime;
	
	UPROPERTY()
	TWeakObjectPtr<ADirectionalLight> CachedDirectionalLight;
};
