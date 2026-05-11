// SplineFollowerComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SplineFollowerComponent.generated.h"

class ACityVehiclePawn;
class ULandscapeSplineControlPoint;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ANTARCITCKIDS_API USplineFollowerComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USplineFollowerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	
	// 월드에서 LandscapeSpline을 찾아 경로 포인트 배열(PathPoints)을 구성
	void BuildPath();
	
	// 기존 경로 포인트들을 Catmull-Rom 스플라인으로 부드럽게 보간하여 일정 간격으로 재샘플링
	void ResampleCatmullRom();
	
	// 현재 위치에서 경로를 따라 Distance만큼 앞의 위치와 그 방향(OutDirection)을 반환
	FVector GetPointAhead(FVector& OutDirection, float Distance) const;
	
	// AheadOffset 거리 앞 지점의 곡률(꺾임 정도)을 각도(라디안)로 추정
	float EstimateCurvature(float AheadOffset) const;
	
	// 곡률을 기반으로 물리적으로 안전한 최대 속도를 계산 (원심력 = 횡 마찰력 공식)
	float ComputeCurveSpeedLimit(float Curvature) const;

private:

	//================= Speed 카테고리 =================
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed", meta=(AllowPrivateAccess="true"))
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed", meta=(AllowPrivateAccess="true"))
	float MinSpeed;

	// 스로틀(Throttle): 엔진이나 모터로 유입되는 공기, 연료, 혹은 전력의 양을 조절하여 기기의 속도나 출력을 제어하는 장치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed", meta=(AllowPrivateAccess="true"))
	float ThrottleGain;

	// 감속 시 보간 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed", meta=(AllowPrivateAccess="true"))
	float DecelRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed", meta=(AllowPrivateAccess="true"))
	float AccelRate;

	
	
	//================= Steering 카테고리 =================
	
	// 기본 전방 주시 거리 (cm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering", meta=(AllowPrivateAccess="true"))
	float LookAheadBase;
	
	// 속도에 따라 주시 거리를 늘리는 비율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering", meta=(AllowPrivateAccess="true"))
	float LookAheadSpeedFactor;

	// 최대 허용 Yaw 오차 (도). 조향 명령 정규화에 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering", meta=(AllowPrivateAccess="true"))
	float MaxYawDelta;

	// 경로 방향(heading) vs 위치 추종(position)의 블렌딩 비율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering", meta=(ClampMin="0", ClampMax="1", AllowPrivateAccess="true"))
	float HeadingWeight;

	// 중앙선 이탈 오차를 보정하는 게인
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering", meta=(AllowPrivateAccess="true"))
	float CrosstrackGain;



	//================= Curvature 카테고리 =================
	
	// 횡 마찰 계수 (속도 제한 계산용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Curvature", meta=(ClampMin="0.1", ClampMax="2.0", AllowPrivateAccess="true"))
	float LateralFriction;

	// 곡률 측정 시 샘플 포인트 간격 (cm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Curvature", meta=(AllowPrivateAccess="true"))
	float CurvatureSampleSpan;

	// 커브가 멀리 있어도 미리 감속을 시작하기 위한 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Curvature", meta=(AllowPrivateAccess="true"))
	float BrakePreviewDist;



	//================= Path 카테고리 =================
	
	// 에디터에서 경로의 시작 CP를 직접 지정 - 기존 코드의 역방향 탐색 제거 (Connections[0]/[1] 방향 혼용 문제 원천 차단)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Path", meta=(AllowPrivateAccess="true"))
	TObjectPtr<ULandscapeSplineControlPoint> StartControlPoint;
	
	// 재샘플링 후 포인트 간 목표 간격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Path", meta=(AllowPrivateAccess="true"))
	float ResampleSpacing;

	// 약한 포인터: 객체가 삭제되어도 크래시 없이 IsValid()로 체크 가능
	UPROPERTY()
	TWeakObjectPtr<ACityVehiclePawn> OwnerPawn;
	
private:
	// 경로를 구성하는 3D 포인트 배열
	TArray<FVector> PathPoints;
	
	// 현재 차량이 통과 중인 포인트 인덱스
	int32 CurrentPointIndex;
	
	// 경로가 루프인지 여부
	bool  bClosedLoop;
	
	// 보간된 목표 속도
	float SmoothedTargetSpeed;
};
