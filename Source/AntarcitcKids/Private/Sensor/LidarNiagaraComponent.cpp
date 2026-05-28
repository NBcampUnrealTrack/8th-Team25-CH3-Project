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
		return;
	}
	
	DeactivateImmediate(); //이전 영역 삭제
	
	
	const int32 IntensityCount = PointCloud.Intensities.Num();
	//강도 - 라이더 센서 어느 거리에서 정보를 수신받았는지 (가까울수록 - 높고, 멀수록 낮습니다)
	//색깔의 강도 
	
	const int32 PointCount = PointCloud.PointCount;
	
	PendingColors.SetNum(PointCloud.PointCount);
	//RESTRICT : 해당 함수 Scope 내에서 같은 메모리를 참조하지 않는다.
	
	
	
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
		this,
		FName("PointCloudPositions"),
		PointCloud.Points
		); 
	//NS에서 PointCloudPositions이라는 유저 변수에 해당 값을 넣어다오.
	
	
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

	
	PendingCount = PointCount;
	

	SetIntParameter(TEXT("CloudPointCount"),PointCount);
	Activate(true);
	
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
	UE_LOG(LogTemp, Warning, TEXT("ToggleIsActiva 발동!"))
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
