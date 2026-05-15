// SplineRoadActor.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Road/SplineRoadActor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

ASplineRoadActor::ASplineRoadActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
	SetRootComponent(SplineComp);
}

void ASplineRoadActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ClearRoad();

	if (!RoadMesh || !SplineComp)
	{
		return;
	}

	const int32 PointCount = SplineComp->GetNumberOfSplinePoints();

	for (int32 i = 0; i < PointCount - 1; ++i)
	{
		USplineMeshComponent* RoadPart = NewObject<USplineMeshComponent>(this);

		AddInstanceComponent(RoadPart);

		RoadPart->SetStaticMesh(RoadMesh);
		RoadPart->SetForwardAxis(ESplineMeshAxis::X);

		// 컴포넌트 자체는 월드 원점 기준으로 둠
		RoadPart->SetWorldLocation(FVector::ZeroVector);
		RoadPart->SetWorldRotation(FRotator::ZeroRotator);
		RoadPart->SetWorldScale3D(FVector::OneVector);

		FVector StartLocation;
		FVector StartTangent;
		FVector EndLocation;
		FVector EndTangent;

		SplineComp->GetLocationAndTangentAtSplinePoint(
			i,
			StartLocation,
			StartTangent,
			ESplineCoordinateSpace::World
		);

		SplineComp->GetLocationAndTangentAtSplinePoint(
			i + 1,
			EndLocation,
			EndTangent,
			ESplineCoordinateSpace::World
		);

		// 도로 메쉬만 Z 오프셋 적용
		StartLocation.Z += MeshZOffset;
		EndLocation.Z += MeshZOffset;

		RoadPart->SetStartAndEnd(
			StartLocation,
			StartTangent,
			EndLocation,
			EndTangent,
			true
		);

		RoadPart->SetSplineUpDir(FVector(0.f, 0.f, 1.f), true);

		RoadPart->RegisterComponent();

		SplineMeshComps.Add(RoadPart);
	}
}

TArray<ASplineRoadActor*> ASplineRoadActor::GetNextRoads() const
{
	// TObjectPtr 배열은 ToRawPtrTArrayUnsafe 또는 명시적 변환 필요
	TArray<ASplineRoadActor*> Result;
	Result.Reserve(NextRoads.Num());

	for (const TObjectPtr<ASplineRoadActor>& Ptr : NextRoads)
	{
		Result.Add(Ptr.Get());
	}

	return Result;
}

void ASplineRoadActor::ClearRoad()
{
	for (TObjectPtr<USplineMeshComponent> RoadPart : SplineMeshComps)
	{
		if (RoadPart)
		{
			RoadPart->DestroyComponent();
		}
	}

	SplineMeshComps.Empty();
}