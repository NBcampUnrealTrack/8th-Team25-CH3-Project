// VehicleWheelFront.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Vehicle/VehicleWheelFront.h"
#include "UObject/ConstructorHelpers.h"


UVehicleWheelFront::UVehicleWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}
