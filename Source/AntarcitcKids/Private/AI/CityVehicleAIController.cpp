// CityVehicleAIController.cpp

#include "AI/CityVehicleAIController.h"
#include "CityVehiclePawn.h"

ACityVehicleAIController::ACityVehicleAIController()
{
	
}

void ACityVehicleAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ControlledVehicle = Cast<ACityVehiclePawn>(InPawn);
}

void ACityVehicleAIController::OnUnPossess()
{
	ControlledVehicle.Reset();
	Super::OnUnPossess();
}