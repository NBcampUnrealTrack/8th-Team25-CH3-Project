// SpeedTrap.cpp

#include "Actor/SpeedTrap.h"
#include "CityVehiclePawn.h"
#include "Components/BoxComponent.h"
#include "Manager/QuestBase.h"



ASpeedTrap::ASpeedTrap()
{
	bIsPersistence = false;
	PawnSpeed = 0;
	StartSpeed = 0.f;
	EndSpeed = 0.f;
	
	// [Note] SpeedTrapQuest 추가되면 아래 주석 제거하기
	//QuestClass = USpeedTrapQuest::StaticClass();
	
	EndGate = CreateDefaultSubobject<UBoxComponent>(TEXT("EndGate"));
	EndGate->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	EndGate->SetupAttachment(SceneRoot);
	EndGate->SetGenerateOverlapEvents(true);
	
	EndGate->OnComponentBeginOverlap.AddDynamic(this,&ASpeedTrap::OnEndGateOverlap);
	
	
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
	if (!IsValid(OtherActor)) return;
	
	if (ACityVehiclePawn* Vehicle = Cast<ACityVehiclePawn>(OtherActor))
	{
		Vehicle->OnHUDSpeedUpdated.AddDynamic(this,&ASpeedTrap::SetPawnSpeed); 
		// [Note] SetPawnSpeed 등록만 하고, 아직 호출 안 됨
		
		PawnSpeed = Vehicle->GetCurrentSpeedKMH(); 
		StartSpeed = PawnSpeed;
		// [Note] 현재 차량 속도를 즉시 직접 조회하여 PawnSpeed를 갱신, 현재 프레임에서 바로 판정 가능
		
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
		
		OnSpeedLimitEntered.Broadcast(SpeedUpperLimit);
	}
}

/*void ASpeedTrap::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnItemEndOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
	if (!IsValid(OtherActor)) return;
	
	if (ACityVehiclePawn* Vehicle = Cast<ACityVehiclePawn>(OtherActor))
	{
		Vehicle->OnHUDSpeedUpdated.RemoveDynamic(this, &ASpeedTrap::SetPawnSpeed);
		
		if (bIsPersistence)
		{
			GetWorld()->GetTimerManager().ClearTimer(PersistenceSpeedCheckTimer);	
		}
	
		if (bIsPersistence && PawnSpeed <= SpeedUpperLimit)
		{
			if (Quest)
				Quest->OnSuccess();
		}
	
		OnSpeedLimitExited.Broadcast();
	}
}*/

void ASpeedTrap::OnEndGateOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor)) return;
	
	if (ACityVehiclePawn* Vehicle = Cast<ACityVehiclePawn>(OtherActor))
	{
		PawnSpeed = Vehicle->GetCurrentSpeedKMH(); 
		EndSpeed = PawnSpeed;
		
		Vehicle->OnHUDSpeedUpdated.RemoveDynamic(this, &ASpeedTrap::SetPawnSpeed);
		

		float FinalSpeed = (StartSpeed + EndSpeed)/2;
		
		if (FinalSpeed <= SpeedUpperLimit)
		{
			if (Quest && Quest->IsQuestEnd())
				Quest->OnSuccess();
		}
	
		OnSpeedLimitExited.Broadcast();
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
		if (Quest && Quest->IsQuestEnd())
			Quest->OnFailed();
	}
}
