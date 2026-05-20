// SplineFollowerComponent.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EntitySystem/MovieSceneEntityLedger.h"
#include "SplineFollowerComponent.generated.h"

class ACityVehiclePawn;
class ASplineRoadActor;
class USplineComponent;

/*
 * 차량이 ASplineRoadActor의 스플라인을 따라 자율 주행하도록 제어하는 컴포넌트.
 * 
 * 책임:
 *   - 주어진 스플라인을 안정적으로 추종 (조향 + 속도 제어)
 *   - 도로 끝 도달 시 정차
 *   - 외부(AI Controller)의 정지/재개 명령 수신
 * 
 * 책임이 아닌 것:
 *   - 시나리오 의사결정 (← ACityVehicleAIController가 담당)
 *   - 도로 간 전환 라우팅 (← ACityVehicleAIController가 담당)
 *   - 직접 물리 입력 (← ACityVehiclePawn이 담당)
 */

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ANTARCITCKIDS_API USplineFollowerComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USplineFollowerComponent();
	
	// ────────────────────────────────────────────────────────
	// 외부(AI Controller) 명령 인터페이스
	// ────────────────────────────────────────────────────────
	
	// 즉시 정지
	UFUNCTION(BlueprintCallable, Category = "SplineFollower|Commands")
	void EmergencyStop();

	// 부드럽게 감속 후 정지
	UFUNCTION(BlueprintCallable, Category = "SplineFollower|Commands")
	void SmoothStop();
	
	// 주행 재개
	UFUNCTION(BlueprintCallable, Category = "SplineFollower|Commands")
	void ResumeDriving();

	// 도로의 기본 속도 설정 (도로 경계 진입 시 호출) - 일반도로 50, 고속도로 110, 주차장 10
	UFUNCTION(BlueprintCallable, Category = "SplineFollower|Commands")
	void SetNormalSpeed(float SpeedKMH);
	
	// 시나리오별 추가 속도 제한 (km/h -> cm/s로 내부 변환)
	UFUNCTION(BlueprintCallable, Category = "SplineFollower|Commands")
	void SetSpeedLimit(float SpeedLimitKMH);
	
	UFUNCTION(BlueprintCallable, Category = "SplineFollower|Commands")
	void ClearSpeedLimit();

	
	// ────────────────────────────────────────────────────────
	// Getter
	// ────────────────────────────────────────────────────────
	
	// 현재 추종 상태 조회
	UFUNCTION(BlueprintPure, Category = "SplineFollower|State")
	bool IsEmergencyStopping() const { return FollowState == ESplineFollowState::EmergencyStopping; }
	
	UFUNCTION(BlueprintPure, Category = "SplineFollower|State")
	bool IsSmoothlyStopping() const { return FollowState == ESplineFollowState::SmoothStopping; }
	
	UFUNCTION(BlueprintPure, Category = "SplineFollower|State")
	bool IsAnyStopActive() const 
	{ 
		return (FollowState == ESplineFollowState::EmergencyStopping) || (FollowState == ESplineFollowState::SmoothStopping); 
	}
	
	UFUNCTION(BlueprintPure, Category = "SplineFollower|State")
	float GetNormalSpeedKHM() const {return RoadNormalSpeedKMH * 0.036f; }
	
	UFUNCTION(BlueprintPure, Category = "SplineFollower|State")
	float GetActiveSpeedLimit() const { return ActiveSpeedLimitKMH; }
	
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	
	// ────────────────────────────────────────────────────────────
	// 초기화 (BeginPlay에서 호출)
	// ────────────────────────────────────────────────────────────
	
	// 추종할 도로를 결정하고 초기 진행 거리를 설정
	void BuildPath();
	
	// CurrentRoad가 미지정이면 가장 가까운 ASplineRoadActor를 찾아 반환
	ASplineRoadActor* FindNearestRoad() const;
	
	// 차량의 현재 위치를 스플라인 상의 거리로 변환하여 ProgressDistance 초기화
	void InitializeProgress();
	
	
	// ────────────────────────────────────────────────────────────
	// 매 프레임 단계 (TickComponent에서 호출)
	// ────────────────────────────────────────────────────────────
	
	// 차량의 실제 위치를 기반으로 ProgressDistance 갱신, 끝 도달 시 true 반환
	bool UpdateProgress(float DeltaTime);

	// 3-way 블렌드(전방주시 + 경로방향 + 중앙선보정)로 조향 입력값(-1~1) 산출
	float ComputeSteeringInput(float CurvHere) const;

	// 곡률 기반 안전속도와 현재속도 차이로 스로틀/브레이크 명령(-1~1) 산출
	// 양수면 스로틀, 음수면 브레이크
	float ComputeThrottleBrakeCommand(float DeltaTime, float CurvHere, float CurvAhead);

	// 산출된 명령값을 Pawn에 전달
	void ApplyControls(float Steering, float ThrottleBrakeCmd);
	
	
	// ────────────────────────────────────────────────────────────
	// 계산 보조 함수
	// ────────────────────────────────────────────────────────────

	// 현재 위치에서 OffsetAhead 거리 앞 지점의 곡률(라디안)을 추정
	float EstimateCurvatureAt(float OffsetAhead) const;
	
	// 곡률을 기반으로 물리적으로 안전한 최대 속도를 계산 (원심력 = 횡 마찰력 공식)
	float ComputeCurveSpeedLimitKMH(float Curvature) const;
	
	// 단위 변환 헬퍼함수
	static float KMHToCMS(float SpeedKMH) { return SpeedKMH / 0.036f; }
	static float CMSToKMH(float SpeedCMS) { return SpeedCMS * 0.036f; }

private:

	//────── Path ──────────────────────────────────────────────────
    
	// 시작 시 추종할 도로 (미지정 시 가장 가까운 도로 자동 탐색)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Path", meta=(AllowPrivateAccess="true"))
	TObjectPtr<ASplineRoadActor> StartingRoad;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Path", meta=(AllowPrivateAccess="true"))
	float EndDetectionThreshold;
	
	//────── Speed ─────────────────────────────────────────────────
	
	// 포인트 : 환경에서 만날 수 있는 모든 속도제한 요소보다 클 것. (변경 지양)
	// 차량의 기계적 최대속도 → 고속도로보다 충분히 크게 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed", meta=(AllowPrivateAccess="true"))
	float VehicleMaxSpeedKMH;
	
	// 도로 속성에 따른 속도 - 고속도로, 자동차전용도로, 일반 도시 도로 등
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed", meta=(AllowPrivateAccess="true"))
	float RoadNormalSpeedKMH;
	
	// 현재는 맑은 날씨 일반 도시 도로 기준의 절대 최소 속도이기 때문에, 추후 소프트하한으로 변경의 여지 있음
	// 곡률 보정용 최저 속도 (급커브에서도 너무 느려지지 않게)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed", meta=(AllowPrivateAccess="true"))
	float MinCurveSpeedKMH;
	
	// (목표속도 - 현재속도) × ThrottleGain → 명령값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed", meta=(AllowPrivateAccess="true"))
	float ThrottleGain;

	// 감속 시 목표 속도 보간 속도 (빠르게)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed", meta=(AllowPrivateAccess="true"))
	float DecelRate;

	// 가속 시 목표 속도 보간 속도 (천천히)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed", meta=(AllowPrivateAccess="true"))
	float AccelRate;
	
	// 외부에서 설정한 활성 속도 제한 (스쿨존, 공사존 등)
	bool bHasActiveSpeedLimit;
	float ActiveSpeedLimitKMH;

	// 보간된 목표 속도(cm/s) - 감속/가속 보간용
	float SmoothedTargetSpeed;
	
	//────── Steering ──────────────────────────────────────────────
    
	// 기본 전방 주시 거리 (cm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering", meta=(AllowPrivateAccess="true"))
	float LookAheadBase;
	
	// 속도에 비례하여 전방 주시 거리를 늘리는 비율 (빠를수록 멀리 봄)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering", meta=(AllowPrivateAccess="true"))
	float LookAheadSpeedFactor;

	// 최대 허용 Yaw 오차 (도). 조향 명령 정규화에 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering", meta=(AllowPrivateAccess="true"))
	float MaxYawDelta;

	// 경로 방향(heading) vs 위치 추종(position)의 블렌딩 비율 (0=위치만, 1=방향만)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering", meta=(ClampMin="0", ClampMax="1", AllowPrivateAccess="true"))
	float HeadingWeight;

	// 중앙선 이탈 거리(cm) → 조향 보정량 변환 게인
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering", meta=(AllowPrivateAccess="true"))
	float CrosstrackGain;

	//────── Curvature ─────────────────────────────────────────────
    
	// 횡 마찰 계수 (0.8 = 마른 아스팔트 근사)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Curvature", meta=(ClampMin="0.1", ClampMax="2.0", AllowPrivateAccess="true"))
	float LateralFriction;

	// 곡률 측정 시 두 샘플 사이 간격 (cm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Curvature", meta=(AllowPrivateAccess="true"))
	float CurvatureSampleSpan;

	// 커브 진입 전 미리 감속을 시작하기 위한 전방 거리 (cm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Curvature", meta=(AllowPrivateAccess="true"))
	float BrakePreviewDist;

private:
	
	enum class ESplineFollowState : uint8
	{
		Driving,			// 정상 주행
		EmergencyStopping,	// 즉시 정차
		SmoothStopping,		// 감속 후 정차
		//Reversing,        // 후진 (주차 시나리오)
		//Parking,          // 주차 동작 중
	};
	
	ESplineFollowState FollowState;

	// 캐시된 소유 Pawn
	UPROPERTY()
	TWeakObjectPtr<ACityVehiclePawn> OwnerPawn;
	
	// 현재 추종 중인 도로의 스플라인 컴포넌트 캐시
	UPROPERTY()
	TWeakObjectPtr<USplineComponent> CurrentSpline;
	
	// 스플라인 상의 현재 진행 거리 (cm, 시작점 기준)
	float ProgressDistance;
	
	// 스플라인 전체 길이 캐시 (BuildPath에서 계산, 도로 변경 전엔 불변)
	float SplineTotalLength;
	
	// 닫힌 루프 여부 캐시
	bool bSplineIsClosedLoop;
};
