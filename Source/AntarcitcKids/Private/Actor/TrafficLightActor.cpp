// TrafficLightActor.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Actor/TrafficLightActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "TimerManager.h"
#include "Manager/TrafficLightQuest.h"
#include "CityVehiclePawn.h"
#include "NiagaraDataInterfaceEmitterBinding.h"
#include "Manager/QuestsTypes.h"

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
	
	QuestClass = UTrafficLightQuest::StaticClass();
}

void ATrafficLightActor::BeginPlay()
{
	Super::BeginPlay();
	
	SwitchToRed();
}

void ATrafficLightActor::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnItemOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (!IsValid(OtherActor)) return;
	
	if (ACityVehiclePawn* Player = Cast<ACityVehiclePawn>(OtherActor))
	{
		if (!IsValid(Player)) return;
		if (CurrentState == ETrafficLightState::Red)
		{
			if (Quest && Quest->IsQuestEnd())
				Quest->OnFailed();
		}
		else
		{
			if (Quest && Quest->IsQuestEnd())
				Quest->OnSuccess();
		}
	}
	
}

void ATrafficLightActor::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnItemEndOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
}

void ATrafficLightActor::SetTrafficLightState(ETrafficLightState NewState)
{
	CurrentState = NewState;

	RedLight->SetVisibility(NewState == ETrafficLightState::Red);
	YellowLight->SetVisibility(NewState == ETrafficLightState::Yellow);
	GreenLight->SetVisibility(NewState == ETrafficLightState::Green);
	
	OnStateChanged.Broadcast(NewState);
}

void ATrafficLightActor::SetQuestInfo()
{
	Super::SetQuestInfo();
	
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