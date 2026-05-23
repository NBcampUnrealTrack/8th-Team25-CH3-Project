// LidarNiagaraComponent.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

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
	void RenderPointCloudNiagara(const FLidarPointCloudData& PointCloud);
	void NiagaraDeactivate();
	void NiagaraActivate();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	UNiagaraSystem* NiagaraAsset;
	
	void ToggleIsActive();
	
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	void BuildColorLUT();
	FLinearColor ColorLUT[256];
	
	TArray<FVector> PendingPositions;
	TArray<FLinearColor> PendingColors;
	int32 PendingCount = 0;
	bool bIsActivate;
	
};
