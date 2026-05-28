// BoxBoundComponent.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/LineBatchComponent.h"
#include "Sensor/CameraSensorTypes.h"
#include "BoxBoundComponent.generated.h"


UCLASS()
class ANTARCITCKIDS_API UBoxBoundComponent : public ULineBatchComponent
{
	GENERATED_BODY()
public:
	void RenderOneBoundingBox(AActor* DetectedActors, FName Tag);

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Color")
	float Thickness;
	
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color")
	float LifeTime;*/
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "LidarSensor")
	TMap<FName, FLinearColor> TagDetectInfos;
	
	void ToggleIsActive();
	
protected:

	virtual void BeginPlay() override;
	bool bIsBoxActive;
};
