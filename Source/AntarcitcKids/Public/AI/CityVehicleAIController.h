// CityVehicleAIController.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CityVehicleAIController.generated.h"

class ACityVehiclePawn;
class USplineFollowerComponent;

/*
 * 자율주행 차량의 의사결정을 담당하는 AI 컨트롤러.
 * 
 * 책임:
 *   - 어떤 도로를 따라갈지 결정
 *   - 시나리오 판단 (신호등, 어린이보호구역, 장애물, 주차 등)
 *   - SplineFollowerComponent에 주행 명령 전달
 * 
 * 책임이 아닌 것:
 *   - 직접 Throttle/Brake/Steering 제어 (← SplineFollowerComponent의 일)
 *   - 물리 시뮬레이션 (← ACityVehiclePawn의 일)
 * 
 * 1차 구현에선 비어있음. 2차 구현 시 시나리오 로직을 여기에 채워넣을 예정.
 */

UCLASS()
class ANTARCITCKIDS_API ACityVehicleAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ACityVehicleAIController();
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	
private:
	// 빙의한 차량 캐시 (UnPossess 시 정리)
	UPROPERTY()
	TWeakObjectPtr<ACityVehiclePawn> ControlledVehicle;
};
