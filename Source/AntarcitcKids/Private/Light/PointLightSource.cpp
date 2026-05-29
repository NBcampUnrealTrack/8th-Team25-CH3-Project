// PointLightSource.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Light/PointLightSource.h"

#include "Components/PointLightComponent.h"

APointLightSource::APointLightSource()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetupAttachment(Root);

	PointLight->SetMobility(EComponentMobility::Stationary);
	PointLight->CastShadows = false;
	PointLight->SetVisibility(false);
}

void APointLightSource::SetLightEnabled(bool bEnabled)
{
	if (PointLight)
	{
		PointLight->SetVisibility(bEnabled);
	}
}