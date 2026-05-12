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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Road")
	TObjectPtr<USplineComponent> SplineComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road")
	TObjectPtr<UStaticMesh> RoadMesh;

	// 도로 메쉬 한 조각의 길이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road")
	float SectionLength = 1000.f;

	// 도로를 스플라인 기준으로 좌우 이동시키고 싶을 때 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road")
	float RoadOffset = 0.f;

	UPROPERTY()
	TArray<TObjectPtr<USplineMeshComponent>> RoadMeshComps;

	void ClearRoad();
};
