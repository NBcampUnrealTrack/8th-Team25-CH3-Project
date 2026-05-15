// Parking.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.


#include "Actor/Parking.h"

#include "Manager/QuestBase.h"
#include "Math/UnitConversion.h"

AParking::AParking()
{
	// [Note] SpeedTrapQuest 추가되면 아래 주석 제거하기
	// QuestClass = UParkingQuest::StaticClass();
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
	GetWorld()->GetTimerManager().ClearTimer(IsCorrectParking);
	
}

void AParking::SetQuestInfo()
{
	Super::SetQuestInfo();
	QuestInfo.QuestName = TEXT("주차");
	QuestInfo.Description = TEXT("주차 선에 맞추어 주차를 진행하시오");
}

void AParking::CheckParking(AActor* Player)
{
	if (!IsValid(Player)) return;
	FBox ParkingBox = GetComponentsBoundingBox();
	FBox PlayerBox = Player->GetComponentsBoundingBox();
	
	PlayerBox.Min.Z = ParkingBox.Min.Z;
	PlayerBox.Max.Z = ParkingBox.Min.Z;
	
	if (!ParkingBox.IsInsideOrOn(PlayerBox.Min) || !ParkingBox.IsInsideOrOn(PlayerBox.Max))
	{
		UE_LOG(LogTemp, Log, TEXT("완전히 주차영역에 들어오지 않음"));
		return;
	}
	
	
	
	float AngleDiff = FMath::RadiansToDegrees(
		FMath::Acos(FMath::Clamp(
			FVector::DotProduct(
				Player->GetActorForwardVector(),
				GetActorForwardVector()),-1.f,1.f)));
	
	
	bool bIsParallel = AngleDiff < LimitAngle || FMath::Abs(AngleDiff - 180.f) < LimitAngle;
	UE_LOG(LogTemp, Warning, TEXT("AngleDiff: %f"), AngleDiff);
	if (bIsParallel)
	{
		if (Quest && Quest->IsQuestEnd())
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


