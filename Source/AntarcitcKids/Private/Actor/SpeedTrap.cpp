// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SpeedTrap.h"
#include "CityVehiclePawn.h"
#include "Manager/QuestBase.h"



ASpeedTrap::ASpeedTrap()
{
	bIsPersistence = false;
	PawnSpeed = 0;
	
}




void ASpeedTrap::SetPawnSpeed(float SpeedKMH)
{
	PawnSpeed = SpeedKMH;
}

void ASpeedTrap::BeginPlay()
{
	Super::BeginPlay();

}





void ASpeedTrap::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnItemOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (ACityVehiclePawn* Player = Cast<ACityVehiclePawn>(OtherActor))
	{
		Player->OnHUDSpeedUpdated.AddDynamic(this,&ASpeedTrap::SetPawnSpeed);
		
	
		
		if (bIsPersistence)
		{
			GetWorld()->GetTimerManager().SetTimer(
				PersistenceSpeedCheckTimer,
				this,
				&ASpeedTrap::PersistentSpeedCheck,
				0.5f,
				true
				);
		}
		else if (PawnSpeed > SpeedUpperLimit)
		{
			if (Quest)
				Quest->OnFailed();
		}
	}

	
	
}

void ASpeedTrap::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnItemEndOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
	if (bIsPersistence)
	{
		GetWorld()->GetTimerManager().ClearTimer(PersistenceSpeedCheckTimer);	
	}
	
	if (bIsPersistence&& PawnSpeed <= SpeedUpperLimit)
	{
		if (Quest)
			Quest->OnSuccess();
	}
	
}

void ASpeedTrap::SetQuestInfo()
{
	Super::SetQuestInfo();
	QuestInfo.QuestName = TEXT("과속 금지");
	QuestInfo.Description = TEXT("일정 속도 이상 금지");
	
}

void ASpeedTrap::PersistentSpeedCheck()
{
	if (PawnSpeed > SpeedUpperLimit)
	{
		if (Quest)
			Quest->OnFailed();
	}
}
