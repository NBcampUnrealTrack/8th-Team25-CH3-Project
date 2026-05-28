// SpeedTrap.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Actor/SpeedTrap.h"
#include "CityVehiclePawn.h"
#include "SNegativeActionButton.h"
#include "Components/BoxComponent.h"
#include "Quest/QuestBase.h"



ASpeedTrap::ASpeedTrap()
{
	bIsPersistence = false;
	PawnSpeedKMH = 0;
	StartSpeedKMH = 0.f;
	EndSpeedKMH = 0.f;
	
	// [Note] SpeedTrapQuest 추가되면 아래 주석 제거하기
	//QuestClass = USpeedTrapQuest::StaticClass();
	
	EndGate = CreateDefaultSubobject<UBoxComponent>(TEXT("EndGate"));
	EndGate->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	EndGate->SetupAttachment(SceneRoot);
	EndGate->SetGenerateOverlapEvents(true);
	
	EndGate->OnComponentBeginOverlap.AddDynamic(this,&ASpeedTrap::OnEndGateOverlap);
	
	AreaCollision->OnComponentBeginOverlap.AddDynamic(this, &ATriggerMissionBase::OnAreaOverlap);
	AreaCollision->OnComponentEndOverlap.AddDynamic(this, &ATriggerMissionBase::OnAreaEndOverlap);
	
}

void ASpeedTrap::SetPawnSpeed(float SpeedKMH)
{
	PawnSpeedKMH = SpeedKMH;
}



void ASpeedTrap::OnAreaOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnAreaOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	OnSpeedLimitEntered.Broadcast(SpeedUpperLimitKHM);
}

void ASpeedTrap::OnAreaEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnAreaEndOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
	
}

void ASpeedTrap::BeginPlay()
{
	Super::BeginPlay();
	SetQuestInfo();
	Quest->OnInitialized(QuestInfo);
}

void ASpeedTrap::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnItemOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (!IsValid(OtherActor)) return;
	UE_LOG(LogTemp,Warning,TEXT("스타트게이트 통과"));
	
	if (ACityVehiclePawn* Vehicle = Cast<ACityVehiclePawn>(OtherActor))
	{
		
		Vehicle->OnHUDSpeedUpdated.AddUniqueDynamic(this,&ASpeedTrap::SetPawnSpeed);
		// [Note] SetPawnSpeed 등록만 하고, 아직 호출 안 됨
		
	
		PawnSpeedKMH = Vehicle->GetCurrentSpeedKMH(); 
		StartSpeedKMH = PawnSpeedKMH;
		// [Note] 현재 차량 속도를 즉시 직접 조회하여 PawnSpeed를 갱신, 현재 프레임에서 바로 판정 가능
		
	
	}
}

void ASpeedTrap::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnItemEndOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
	if (!IsValid(OtherActor)) return;
	
	if (ACityVehiclePawn* Vehicle = Cast<ACityVehiclePawn>(OtherActor))
	{
		Vehicle->OnHUDSpeedUpdated.RemoveDynamic(this, &ASpeedTrap::SetPawnSpeed);
		
		
	}
}

//엔드 게이트의 오버랩에 따라, 임계 속력 이상이면 실패, 아니면 성공으로 판정
void ASpeedTrap::OnEndGateOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	UE_LOG(LogTemp,Warning,TEXT("엔드게이트 통과"));
	if (!IsValid(OtherActor)) return;
	
	if (ACityVehiclePawn* Vehicle = Cast<ACityVehiclePawn>(OtherActor))
	{
		PawnSpeedKMH = Vehicle->GetCurrentSpeedKMH(); 
		EndSpeedKMH = PawnSpeedKMH;
		
		Vehicle->OnHUDSpeedUpdated.RemoveDynamic(this, &ASpeedTrap::SetPawnSpeed);
		

		float FinalSpeed = (StartSpeedKMH + EndSpeedKMH)/2;
		
		OnSpeedLimitExited.Broadcast();
		if (FinalSpeed <= SpeedUpperLimitKHM)
		{
			
			if (Quest && Quest->IsQuestEnd())
			{
				Quest->OnSuccess();
			}
			
		}
	
		
	}

}

void ASpeedTrap::SetQuestInfo()
{
	QuestInfo.QuestType = EQuestClass::SpeedTrap;
	QuestInfo.QuestName = TEXT("과속 금지");
	QuestInfo.Description = TEXT("일정 속도 이상 금지");
	Super::SetQuestInfo();

	
}

void ASpeedTrap::PersistentSpeedCheck()
{
	
	if (PawnSpeedKMH > SpeedUpperLimitKHM)
	{
		if (Quest && Quest->IsQuestEnd())
			Quest->OnFailed();
	}
}
