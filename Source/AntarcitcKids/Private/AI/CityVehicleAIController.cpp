// CityVehicleAIController.cpp

#include "AI/CityVehicleAIController.h"
#include "CityVehiclePawn.h"


// 커스텀 로그 카테고리를 정의
DEFINE_LOG_CATEGORY_STATIC(LogCityVehicleAIController, Log, All);


ACityVehicleAIController::ACityVehicleAIController()
{
	
}

void ACityVehicleAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogCityVehicleAIController, Log, TEXT("AI Controller Possessed: %s"), *InPawn->GetName());
	ControlledVehicle = Cast<ACityVehiclePawn>(InPawn);
}

void ACityVehicleAIController::OnUnPossess()
{
	ControlledVehicle.Reset();
	Super::OnUnPossess();
}