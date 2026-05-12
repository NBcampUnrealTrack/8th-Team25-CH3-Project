#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SplineRoadActor.generated.h"

class USplineComponent;
class USplineMeshComponent;
class UStaticMesh;

UCLASS()
class ANTARCITCKIDS_API ASplineRoadActor : public AActor
{
	GENERATED_BODY()

public:
	ASplineRoadActor();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Road")
	TObjectPtr<USplineComponent> SplineComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road")
	TObjectPtr<UStaticMesh> RoadMesh;

	UPROPERTY()
	TArray<TObjectPtr<USplineMeshComponent>> SplineMeshComps;

	void ClearRoad();
};