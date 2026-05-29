// BoxBoundComponent.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Sensor/BoxBoundComponent.h"
#include "MeshPaintVisualize.h"



//탐색된 엑터에 대해서 BoundingBox 렌더링
void UBoxBoundComponent::RenderOneBoundingBox(AActor* DetectedActor, FName Tag)
{

	

	if (!DetectedActor || !bIsBoxActive)
	{
		
		return;
	}
	
	
	UE_LOG(LogTemp, Warning, TEXT("RenderBoundBox Successful"));
	float LifeTime = 15.f;

	
	UStaticMeshComponent* MeshComp = DetectedActor->FindComponentByClass<UStaticMeshComponent>();
	FBox LocalBox;
	if (!MeshComp)
	{
		USkeletalMeshComponent* MeshCompSK = DetectedActor->FindComponentByClass<USkeletalMeshComponent>();
		FBoxSphereBounds SkelBounds = MeshCompSK->CalcBounds(MeshCompSK->GetComponentTransform());
		LocalBox = SkelBounds.GetBox();
	}
	else
	{
		
		LocalBox = MeshComp->GetStaticMesh()->GetBoundingBox();
	}
	
	
	
		
		FVector Min = LocalBox.Min;
		FVector Max = LocalBox.Max;
		
		FVector LocalV[8] = {
			{Min.X, Min.Y, Min.Z},
			{Max.X, Min.Y, Min.Z},
			{Max.X, Max.Y, Min.Z},
			{Min.X, Max.Y, Min.Z},
			{Min.X, Min.Y, Max.Z},
			{Max.X, Min.Y, Max.Z},
			{Max.X, Max.Y, Max.Z},
			{Min.X, Max.Y, Max.Z}
		};
	
		FTransform MeshTransform = MeshComp->GetComponentTransform();
		FVector V[8];
		for (int32 i = 0; i < 8; i++)
		{
			V[i] = MeshTransform.TransformPosition(LocalV[i]);
		}
		
		FLinearColor Color;
		
		if (TagDetectInfos.Find(Tag))
		{
			Color =  *TagDetectInfos.Find(Tag); 
		}
		else
		{
			Color = FLinearColor(1,1,1);
		}
		FColor LineColor = Color.ToFColor(true);
		
		DrawLine(V[0], V[1], LineColor, SDPG_Foreground, Thickness, LifeTime);
		DrawLine(V[1], V[2], LineColor, SDPG_Foreground, Thickness, LifeTime);
		DrawLine(V[2], V[3], LineColor, SDPG_Foreground, Thickness, LifeTime);
		DrawLine(V[3], V[0], LineColor, SDPG_Foreground, Thickness, LifeTime);
		DrawLine(V[4], V[5], LineColor, SDPG_Foreground, Thickness, LifeTime);
		DrawLine(V[5], V[6], LineColor, SDPG_Foreground, Thickness, LifeTime);
		DrawLine(V[6], V[7], LineColor, SDPG_Foreground, Thickness, LifeTime);
		DrawLine(V[7], V[4], LineColor, SDPG_Foreground, Thickness, LifeTime);
		DrawLine(V[0], V[4], LineColor, SDPG_Foreground, Thickness, LifeTime);
		DrawLine(V[1], V[5], LineColor, SDPG_Foreground, Thickness, LifeTime);
		DrawLine(V[2], V[6], LineColor, SDPG_Foreground, Thickness, LifeTime);
		DrawLine(V[3], V[7], LineColor, SDPG_Foreground, Thickness, LifeTime);

}


void UBoxBoundComponent::ToggleIsActive()
{
	BatchedLines.Empty();
	bIsBoxActive = !bIsBoxActive;
}

void UBoxBoundComponent::BeginPlay()
{
	Super::BeginPlay();
	bIsBoxActive = false;

	
}
