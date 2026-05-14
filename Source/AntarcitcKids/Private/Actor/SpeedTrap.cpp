// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SpeedTrap.h"



void ASpeedTrap::BeginPlay()
{
	Super::BeginPlay();
}

bool ASpeedTrap::IsSpeedOver()
{
}

bool ASpeedTrap::IsSpeedLower()
{
}


void ASpeedTrap::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnItemOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	
}

void ASpeedTrap::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnItemEndOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
}

void ASpeedTrap::SetQuestInfo()
{
	Super::SetQuestInfo();
	QuestInfo.QuestName = TEXT("과속 금지");
	QuestInfo.Description = TEXT("일정 속도 이상 금지");
	
}

void ASpeedTrap::PersistentSpeedCheck()
{
	if (IsSpeed)
}
