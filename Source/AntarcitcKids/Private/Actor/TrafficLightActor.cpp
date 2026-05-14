#include "Actor/TrafficLightActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
#include "Manager/TrafficLightQuest.h"
#include "CityVehiclePawn.h"
#include "NiagaraSystem.h"
#include "Manager/QuestsTypes.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"

ATrafficLightActor::ATrafficLightActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RedLight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RedLight"));
	RedLight->SetupAttachment(SceneRoot);

	YellowLight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("YellowLight"));
	YellowLight->SetupAttachment(SceneRoot);

	GreenLight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GreenLight"));
	GreenLight->SetupAttachment(SceneRoot);
	
	CurrentState = ETrafficLightState::Red;
}

void ATrafficLightActor::BeginPlay()
{
	Super::BeginPlay();
	
	SwitchToRed();
	SetQuestInfo();

}

void ATrafficLightActor::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACityVehiclePawn* Player = Cast<ACityVehiclePawn>(OtherActor))
	{
		if (CurrentState == ETrafficLightState::Red)
		{
			if (Quest)
				Quest->OnFailed();
		}
		else
		{
			if (Quest)
				Quest->OnSuccess();
		}
	}
	
}

void ATrafficLightActor::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void ATrafficLightActor::SetTrafficLightState(ETrafficLightState NewState)
{
	CurrentState = NewState;

	RedLight->SetVisibility(NewState == ETrafficLightState::Red);
	YellowLight->SetVisibility(NewState == ETrafficLightState::Yellow);
	GreenLight->SetVisibility(NewState == ETrafficLightState::Green);
}

void ATrafficLightActor::SetQuestInfo()
{
	if (NiagaraAnchor)
	{
		QuestInfo.QuestLocation = NiagaraAnchor->GetComponentLocation();	
	}
	

	QuestInfo.QuestName = TEXT("신호등 준수 퀘스트");
	QuestInfo.Description = TEXT("신호등 알맞게 진행");
}

void ATrafficLightActor::SwitchToRed()
{
	SetTrafficLightState(ETrafficLightState::Red);

	GetWorldTimerManager().SetTimer(
		TrafficLightTimerHandle,
		this,
		&ATrafficLightActor::SwitchToGreen,
		RedDuration,
		false
	);
}

void ATrafficLightActor::SwitchToGreen()
{
	SetTrafficLightState(ETrafficLightState::Green);

	GetWorldTimerManager().SetTimer(
		TrafficLightTimerHandle,
		this,
		&ATrafficLightActor::SwitchToYellow,
		GreenDuration,
		false
	);
}

void ATrafficLightActor::SwitchToYellow()
{
	SetTrafficLightState(ETrafficLightState::Yellow);

	GetWorldTimerManager().SetTimer(
		TrafficLightTimerHandle,
		this,
		&ATrafficLightActor::SwitchToRed,
		YellowDuration,
		false
	);
}