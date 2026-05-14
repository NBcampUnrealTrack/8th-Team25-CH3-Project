// Fill out your copyright notice in the Description page of Project Settings.


#include "Sensor/LidarNiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Sensor/CameraSensorTypes.h"
#include "NiagaraSystem.h"

#include "ScreenPass.h"

ULidarNiagaraComponent::ULidarNiagaraComponent()
{

}

void ULidarNiagaraComponent::RenderPointCloudNiagara(const FLidarPointCloudData& PointCloud)
{
	if (!GetSystemInstance()) return;
	DeactivateImmediate(); 
	const int32 IntensityCount = PointCloud.Intensities.Num();
	const int32 PointCount = PointCloud.PointCount;
	
	PendingColors.SetNum(PointCloud.PointCount);
	//RESTRICT : 해당 함수 Scope 내에서 같은 메모리를 참조하지 않는다.

	
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
		this,
		FName("PointCloudPoints"),
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
}

void ULidarNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

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
