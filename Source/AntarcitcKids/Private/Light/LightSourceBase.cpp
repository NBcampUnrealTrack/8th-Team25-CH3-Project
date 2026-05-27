// LightSourceBase.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Light/LightSourceBase.h"

ALightSourceBase::ALightSourceBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALightSourceBase::SetLightEnabled(bool bEnabled)
{
}