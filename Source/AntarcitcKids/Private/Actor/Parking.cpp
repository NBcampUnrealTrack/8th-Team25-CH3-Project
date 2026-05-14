// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Parking.h"

#include "Manager/QuestBase.h"
#include "Math/UnitConversion.h"

AParking::AParking()
{
}

void AParking::BeginPlay()
{
	Super::BeginPlay();
}

void AParking::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                             int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnItemOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	StartParkingCheck(OtherActor);
	
}

void AParking::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	Super::OnItemEndOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
	
}

void AParking::SetQuestInfo()
{
	Super::SetQuestInfo();
	QuestInfo.QuestName = TEXT("주차");
	QuestInfo.Description = TEXT("주차 선에 맞추어 주차를 진행하시오");
}

void AParking::CheckParking(AActor* Player)
{
	if (!Player) return;
	
	float AngleDiff = FMath::RadiansToDegrees(
		FMath::Acos(FMath::Clamp(
			FVector::DotProduct(
				Player->GetActorForwardVector(),
				GetActorForwardVector()),-1.f,1.f)));
	
	
	bool bIsParallel = AngleDiff < LimitAngle || FMath::Abs(AngleDiff - 180.f) < LimitAngle;
	
	if (bIsParallel)
	{
		if (Quest)
			Quest->OnSuccess();
	}

	
	
	
}

void AParking::StartParkingCheck(AActor* Player)
{
	FTimerDelegate StartParkingCheck;
	StartParkingCheck.BindUObject(this,&AParking::CheckParking,Player);
	
	GetWorld()->GetTimerManager().SetTimer(
	IsCorrectParking,
	StartParkingCheck,
	0.5f,
	true);
	
}


