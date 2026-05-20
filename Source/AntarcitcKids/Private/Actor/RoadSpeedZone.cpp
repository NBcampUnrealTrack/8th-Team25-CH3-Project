// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/RoadSpeedZone.h"

#include "CityVehiclePawn.h"

ARoadSpeedZone::ARoadSpeedZone()
	: NormalSpeedKMH(50.f)
{
}

void ARoadSpeedZone::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnItemOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	if (ACityVehiclePawn* Vehicle = Cast<ACityVehiclePawn>(OtherActor))
	{
		OnNormalSpeedChanged.Broadcast(NormalSpeedKMH);
	}
}