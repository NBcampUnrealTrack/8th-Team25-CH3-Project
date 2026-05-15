// CyberTruckWheelRear.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Vehicle/CyberTruckWheelRear.h"

UCyberTruckWheelRear::UCyberTruckWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedBySteering = false;
	
	WheelRadius = 46.0f;
	WheelWidth = 45.0f;
	FrictionForceMultiplier = 3.5f;
	SlipThreshold = 100.0f;
	SkidThreshold = 100.0f;
	MaxSteerAngle = 0.0f;
	MaxHandBrakeTorque = 6000.0f;
	
	SuspensionMaxRaise = 8.0f;
	SuspensionMaxDrop = 8.0f;
	SpringRate = 800.0f;
	SpringPreload = 180.0f;
	SuspensionDampingRatio = 1.5f;
	SuspensionSmoothing = 6;
	RollbarScaling = 0.7f;
}