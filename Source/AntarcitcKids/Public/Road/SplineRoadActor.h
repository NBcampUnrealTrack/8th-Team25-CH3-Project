#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Road/RoadTypes.h"
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
	
	UFUNCTION(BlueprintPure, Category = "Road")
	USplineComponent* GetSplineComponent() const { return SplineComp; }
	
	UFUNCTION(BlueprintPure, Category = "Road|Identity")
	FName GetRoadID() const { return RoadID; }
	
	// 이 도로 끝에서 갈 수 있는 다음 도로 후보들
	// 1개면 자동 연결, 2개 이상이면 분기점 (이번 과제에서는 자동 연결 1개로)
	UFUNCTION(BlueprintPure, Category = "Road|Routing")
	TArray<ASplineRoadActor*> GetNextRoads() const;


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Road")
	TObjectPtr<USplineComponent> SplineComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road")
	TObjectPtr<UStaticMesh> RoadMesh;

	UPROPERTY()
	TArray<TObjectPtr<USplineMeshComponent>> SplineMeshComps;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Road|Identity")
	FName RoadID;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Road|Routing")
	TArray<TObjectPtr<ASplineRoadActor>> NextRoads;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Road|Properties")
	ERoadCategory Category = ERoadCategory::Unspecified;

	// 카테고리 기본 속도 무시하고 특정 값(cm/s) 강제 적용이 필요한 경우 
	// -1 디폴트 값 의미: 카테고리 기본값 사용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Road|Properties", meta=(ClampMin="-1", UIMin="-1"))
	float SpeedLimitOverride = -1.f;
	
	// 주차장 진입로 끝 지점 (주차 시나리오용)
	// nullptr이면 일반 도로로 간주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Road|Properties")
	TObjectPtr<AActor> ParkingTarget = nullptr;
	
	void ClearRoad();
};