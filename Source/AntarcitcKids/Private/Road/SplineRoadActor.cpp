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

	const float SplineLength = SplineComp->GetSplineLength();

	if (SplineLength <= 0.f || SectionLength <= 0.f)
	{
		return;
	}

	const int32 SectionCount = FMath::CeilToInt(SplineLength / SectionLength);
	
	for (int32 i = 0; i < SectionCount; ++i)
	{
		const float StartDistance = i * SectionLength;
		const float EndDistance = FMath::Min((i + 1) * SectionLength, SplineLength);

		FVector StartLocation = SplineComp->GetLocationAtDistanceAlongSpline(
			StartDistance,
			ESplineCoordinateSpace::Local
		);

		FVector StartTangent = SplineComp->GetTangentAtDistanceAlongSpline(
			StartDistance,
			ESplineCoordinateSpace::Local
		);

		FVector EndLocation = SplineComp->GetLocationAtDistanceAlongSpline(
			EndDistance,
			ESplineCoordinateSpace::Local
		);

		FVector EndTangent = SplineComp->GetTangentAtDistanceAlongSpline(
			EndDistance,
			ESplineCoordinateSpace::Local
		);
		
		USplineMeshComponent* RoadPart = NewObject<USplineMeshComponent>(this);
		
		RoadPart->SetWorldLocation(GetActorLocation());
		RoadPart->SetWorldRotation(GetActorRotation());
		RoadPart->SetWorldScale3D(GetActorScale3D());

		RoadPart->SetStaticMesh(RoadMesh);
		
		RoadPart->SetForwardAxis(ESplineMeshAxis::X);
		

		// 스플라인 기준 좌우 오프셋
		RoadPart->SetStartOffset(FVector2D(0.f, RoadOffset));
		RoadPart->SetEndOffset(FVector2D(0.f, RoadOffset));

		RoadPart->SetStartAndEnd(
			StartLocation,
			StartTangent,
			EndLocation,
			EndTangent,
			true
		);

		RoadPart->SetSplineUpDir(FVector(0.f, 0.f, 1.f), true);

		RoadPart->RegisterComponent();

		RoadMeshComps.Add(RoadPart);
	}
}

void ASplineRoadActor::ClearRoad()
{
	for (TObjectPtr<USplineMeshComponent> RoadPart : RoadMeshComps)
	{
		if (RoadPart)
		{
			RoadPart->DestroyComponent();
		}
	}

	RoadMeshComps.Empty();
}