// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "CameraSensorTypes.h"
#include "LidarNiagaraComponent.generated.h"


UCLASS()
class ANTARCITCKIDS_API ULidarNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
	
public:
	ULidarNiagaraComponent();
	/*void RenderPointCloudNiagara(const FLidarPointCloudData& PointCloud);*/
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	UNiagaraSystem* NiagaraAsset;
	
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	void BuildColorLUT();
	FLinearColor ColorLUT[256];
	
	TArray<FVector> PendingPositions;
	TArray<FLinearColor> PendingColors;
	int32 PendingCount = 0;
	
};
