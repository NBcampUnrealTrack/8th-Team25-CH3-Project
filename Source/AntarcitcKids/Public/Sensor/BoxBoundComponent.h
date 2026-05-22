// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/LineBatchComponent.h"
#include "Sensor/CameraSensorTypes.h"
#include "BoxBoundComponent.generated.h"

/**
 * 
 */
UCLASS()
class ANTARCITCKIDS_API UBoxBoundComponent : public ULineBatchComponent
{
	GENERATED_BODY()
public:
	void RenderBoundingBox(TArray<AActor*> DetectedActor, FName Tag);
	void Hide();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Color")
	float Thickness;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color")
	float LifeTime;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "LidarSensor")
	TMap<FName, FLinearColor> TagDetectInfos;
	
protected:

	virtual void BeginPlay() override;
};
