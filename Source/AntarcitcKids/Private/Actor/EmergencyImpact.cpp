// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/EmergencyImpact.h"

#include "Manager/QuestBase.h"

AEmergencyImpact::AEmergencyImpact()
{
	bIsImpact = false;
}

void AEmergencyImpact::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnItemOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	if (!IsValid(OtherActor)) return;

	StartCheckImpact(OtherActor);
	
	
}

void AEmergencyImpact::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
	Super::OnItemEndOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
	
	if (!IsValid(OtherActor)) return;
	GetWorld()->GetTimerManager().ClearTimer(ImpactCheckTimer);
	
	if (bIsImpact)
	{
		if (IsValid(Quest))
			Quest->OnFailed();
	}
	else
	{
		if (IsValid(Quest))
			Quest->OnSuccess();
	}

}

void AEmergencyImpact::CheckImpact(AActor* OtherActor)
{
	if (!IsValid(OtherActor)) return;
	
	FVector CurrentLocation = GetActorLocation();
	FVector ImpactorLocation = OtherActor->GetActorLocation();
	
	if (FVector::Dist(CurrentLocation,ImpactorLocation) <= ImpactDistance)
	{
		bIsImpact = true;
	}
	UE_LOG(LogTemp,Warning, TEXT("충돌 여부 : %d"),bIsImpact);
}

void AEmergencyImpact::StartCheckImpact(AActor* OtherActor)
{
	FTimerDelegate StartImpactCheck;
	bIsImpact = false;
	StartImpactCheck.BindUObject(this,&AEmergencyImpact::CheckImpact,OtherActor);
	
	GetWorld()->GetTimerManager().SetTimer(ImpactCheckTimer,StartImpactCheck,0.5f,true);
}




