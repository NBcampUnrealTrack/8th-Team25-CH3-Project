// CyberTruckWheelFront.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Vehicle/CyberTruckWheelFront.h"

UCyberTruckWheelFront::UCyberTruckWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	
	WheelRadius = 45.0f;
	WheelWidth = 40.0f;
	FrictionForceMultiplier = 2.5f;
	MaxSteerAngle = 35.0f;
	
	MaxBrakeTorque = 6000.0f;
	MaxHandBrakeTorque = 6000.0f;
	
	SuspensionMaxRaise = 8.0f;
	SuspensionMaxDrop = 8.0f;
	SpringRate = 700.0f;
	SpringPreload = 150.0f;
	SuspensionDampingRatio = 1.5f;
	SuspensionSmoothing = 6;
	RollbarScaling = 0.6f;
}