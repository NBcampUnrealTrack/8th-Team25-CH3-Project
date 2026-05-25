#include "Light/LightSourceBase.h"

ALightSourceBase::ALightSourceBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALightSourceBase::SetLightEnabled(bool bEnabled)
{
}