// Fill out your copyright notice in the Description page of Project Settings.


#include "Sensor/BoxBoundComponent.h"

#include "MeshPaintVisualize.h"

void UBoxBoundComponent::RenderBoundingBox(TArray<AActor*> DetectedActors, FName Tag)
{
	
	/*UE_LOG(LogTemp, Warning, TEXT("DetectedActor Activate"));*/
	if (DetectedActors.IsEmpty())
	{
		/*UE_LOG(LogTemp, Warning, TEXT("DetectedActor Empty()"));*/
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DetectedActor Has Something"));
		
	}
	Flush();
	
	

	for (AActor* DetectedActor: DetectedActors)
	{
		FBox Box = DetectedActor->GetComponentsBoundingBox(true);
		FVector Min = Box.Min;
		FVector Max = Box.Max;
		
		FVector V[8] = {
			{Min.X, Min.Y, Min.Z},
			{Max.X, Min.Y, Min.Z},
			{Max.X, Max.Y, Min.Z},
			{Min.X, Max.Y, Min.Z},
			{Min.X, Min.Y, Max.Z},
			{Max.X, Min.Y, Max.Z},
			{Max.X, Max.Y, Max.Z},
			{Min.X, Max.Y, Max.Z}
		};
		
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
		
		DrawLine(V[0], V[1], LineColor, 0, Thickness, LifeTime);
		DrawLine(V[1], V[2], LineColor, 0, Thickness, LifeTime);
		DrawLine(V[2], V[3], LineColor, 0, Thickness, LifeTime);
		DrawLine(V[3], V[0], LineColor, 0, Thickness, LifeTime);
		DrawLine(V[4], V[5], LineColor, 0, Thickness, LifeTime);
		DrawLine(V[5], V[6], LineColor, 0, Thickness, LifeTime);
		DrawLine(V[6], V[7], LineColor, 0, Thickness, LifeTime);
		DrawLine(V[7], V[4], LineColor, 0, Thickness, LifeTime);
		DrawLine(V[0], V[4], LineColor, 0, Thickness, LifeTime);
		DrawLine(V[1], V[5], LineColor, 0, Thickness, LifeTime);
		DrawLine(V[2], V[6], LineColor, 0, Thickness, LifeTime);
		DrawLine(V[3], V[7], LineColor, 0, Thickness, LifeTime);
	}
}



void UBoxBoundComponent::BeginPlay()
{
	Super::BeginPlay();
	

	
}
