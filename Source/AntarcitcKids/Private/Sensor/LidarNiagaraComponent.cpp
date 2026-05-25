// LidarNiagaraComponent.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Sensor/LidarNiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Sensor/CameraSensorTypes.h"
#include "NiagaraSystem.h"

#include "ScreenPass.h"

ULidarNiagaraComponent::ULidarNiagaraComponent()
{
	UE_LOG(LogTemp, Warning, TEXT("RenderPointCloudNiagara 생성자 발동"));
}

void ULidarNiagaraComponent::RenderPointCloudNiagara(const FLidarPointCloudData& PointCloud)
{
	if (!bIsActivate)
	{
		UE_LOG(LogTemp, Warning, TEXT("bIsActive: %d"), bIsActivate);
		return;
	}
	
	/*UE_LOG(LogTemp, Warning, TEXT("RenderPointCloudNiagara 발동"));*/
	DeactivateImmediate(); 
	const int32 IntensityCount = PointCloud.Intensities.Num();
	const int32 PointCount = PointCloud.PointCount;
	
	PendingColors.SetNum(PointCloud.PointCount);
	//RESTRICT : 해당 함수 Scope 내에서 같은 메모리를 참조하지 않는다.
	
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
		this,
		FName("PointCloudPositions"),
		PointCloud.Points
		);
	
	
	//PointCloud 색깔 설정
	for (int32 i=0; i<PointCount;i++)
	{
		const float Intensity = (i < IntensityCount) ? PointCloud.Intensities[i] : 0.5f;
		PendingColors[i] = ColorLUT[
		static_cast<uint8>(FMath::Clamp(Intensity * 255.f, 0.f, 255.f))
	];
	}
	
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayColor(
		this,
		FName("PointCloudColors"),
		PendingColors
		);
	
	/*for (FLinearColor Color: PendingColors)
	{
		UE_LOG(LogTemp, Warning, TEXT("Color: R=%.2f G=%.2f B=%.2f A=%.2f"),
	Color.R, Color.G, Color.B, Color.A);
	}*/
	
	PendingCount = PointCount;
	

	SetIntParameter(TEXT("CloudPointCount"),PointCount);
	Activate(true);
	
	if (this->IsActive())
	{
		UE_LOG(LogTemp, Warning, TEXT("Niagara 작동 !"));
	}
}

void ULidarNiagaraComponent::NiagaraDeactivate()
{
	bIsActivate = false;
}

void ULidarNiagaraComponent::NiagaraActivate()
{
	bIsActivate = true;
}

void ULidarNiagaraComponent::ToggleIsActive()
{
	DeactivateImmediate();
	bIsActivate = !bIsActivate;
}

void ULidarNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("RenderPointCloudNiagara BeginPlay발동"));
	BuildColorLUT();
	Activate();
}

void ULidarNiagaraComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void ULidarNiagaraComponent::BuildColorLUT()
{
	UE_LOG(LogTemp, Warning, TEXT("BuildColorLUT 발동"));
	const FLinearColor DarkGreen(0.0f, 0.3f, 0.0f, 1.0f);
	const FLinearColor Bright(0.0f, 1.0f, 0.2f, 1.0f);
	
	for (int32 i = 0; i < 256; ++i)
	{
		ColorLUT[i] = FMath::Lerp(
			DarkGreen,
			Bright,
			static_cast<float>(i) / 255.f
		);
	}
}
