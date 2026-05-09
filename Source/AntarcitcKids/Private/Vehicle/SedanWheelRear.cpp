#include "Vehicle/SedanWheelRear.h"

USedanWheelRear::USedanWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedBySteering = false;
	
	WheelRadius = 40.0f;
	WheelWidth = 40.0f;
	FrictionForceMultiplier = 4.0;
	SlipThreshold = 100.0f;
	SkidThreshold = 100.0f;
	MaxSteerAngle = 0.0f;
	MaxHandBrakeTorque = 6000.0f;
	
	SuspensionMaxRaise = 5.0f;
	SuspensionMaxDrop = 5.0f;
	SpringRate = 550.0f;
	SpringPreload = 100.0f;
	SuspensionDampingRatio = 1.5f;
	SuspensionSmoothing = 6;
	RollbarScaling = 0.9f;
}
