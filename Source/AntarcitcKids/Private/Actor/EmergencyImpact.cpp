// EmergencyImpact.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Actor/EmergencyImpact.h"
#include "Components/SphereComponent.h"
#include "Manager/QuestBase.h"


AEmergencyImpact::AEmergencyImpact()
{
	bIsImpact = false;
	
	ImpactCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ImpactCollision"));
	ImpactCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ImpactCollision->SetupAttachment(SceneRoot);
	ImpactCollision->SetGenerateOverlapEvents(true);
	
	ImpactCollision->OnComponentBeginOverlap.AddDynamic(this, &AEmergencyImpact::OnImpactOverlap);
}

void AEmergencyImpact::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnItemOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	if (!IsValid(OtherActor)) return;
	
	
}

void AEmergencyImpact::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
	Super::OnItemEndOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
	
	if (!IsValid(OtherActor)) return;
	
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
	bIsImpact = false;

}

void AEmergencyImpact::OnImpactOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bIsImpact = true;
}






