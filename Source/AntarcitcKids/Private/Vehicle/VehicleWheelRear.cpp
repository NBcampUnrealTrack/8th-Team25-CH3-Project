#include "Vehicle/VehicleWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UVehicleWheelRear::UVehicleWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}
