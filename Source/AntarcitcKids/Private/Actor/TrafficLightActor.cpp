// TrafficLightActor.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Actor/TrafficLightActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "TimerManager.h"
#include "Quest/TrafficLightQuest.h"
#include "CityVehiclePawn.h"
#include "NiagaraDataInterfaceEmitterBinding.h"
#include "Components/BoxComponent.h"
#include "Quest/QuestsTypes.h"

ATrafficLightActor::ATrafficLightActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	RedLight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RedLight"));
	RedLight->SetupAttachment(SceneRoot);

	YellowLight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("YellowLight"));
	YellowLight->SetupAttachment(SceneRoot);

	GreenLight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GreenLight"));
	GreenLight->SetupAttachment(SceneRoot);
	
	CurrentState = ETrafficLightState::Red;
	
	QuestClass = UTrafficLightQuest::StaticClass();
	
	AreaCollision->OnComponentBeginOverlap.AddDynamic(this, &ATriggerMissionBase::OnAreaOverlap);
	AreaCollision->OnComponentEndOverlap.AddDynamic(this, &ATriggerMissionBase::OnAreaEndOverlap);
	
	IsVehicleInArea = false;
	
}

void ATrafficLightActor::BeginPlay()
{
	Super::BeginPlay();
	SetQuestInfo();
	Quest->OnInitialized(QuestInfo);
	SwitchToRed();
}

//오버랩 시 최초 1회 신호 정보를 보낸다.
void ATrafficLightActor::OnAreaOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnAreaOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	if (ACityVehiclePawn* Vehicle = Cast<ACityVehiclePawn>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT(">>> Vehicle ENTERED traffic light ZONE"));
		OnStateChanged.Broadcast(CurrentState);
		IsVehicleInArea = true;
	}
}

//오버랩 종료 시 신호등 영역을 벗어난다.
void ATrafficLightActor::OnAreaEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnAreaEndOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
	
	if (ACityVehiclePawn* Vehicle = Cast<ACityVehiclePawn>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT(">>> Vehicle EXITED traffic light ZONE"));
		IsVehicleInArea = false;
	}
}

//오버랩 시, 빨간 색이 아니라면 미션 성공, 빨간색이면 실패
void ATrafficLightActor::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnItemOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (!IsValid(OtherActor)) return;
	
	if (ACityVehiclePawn* Vehicle = Cast<ACityVehiclePawn>(OtherActor))
	{
		if (!IsValid(Vehicle)) return;
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
		
		UE_LOG(LogTemp, Warning, TEXT(">>> Vehicle ENTERED traffic light"));
	}
}

//신호등의 색깔을 바꾸고, 델리게이트 함수를 통해 알린다.
void ATrafficLightActor::SetTrafficLightState(ETrafficLightState NewState)
{
	CurrentState = NewState;

	RedLight->SetVisibility(NewState == ETrafficLightState::Red);
	YellowLight->SetVisibility(NewState == ETrafficLightState::Yellow);
	GreenLight->SetVisibility(NewState == ETrafficLightState::Green);
	
	if (IsVehicleInArea)
	{
		OnStateChanged.Broadcast(NewState);
	}
}


//Quest 내부 내용을 활성화한다.
void ATrafficLightActor::SetQuestInfo()
{
	QuestInfo.QuestType = EQuestClass::TrafficSignal;
	QuestInfo.QuestName = TEXT("신호등 준수 퀘스트");
	QuestInfo.Description = TEXT("신호등 알맞게 진행");
	Super::SetQuestInfo();

}


//각 신호 시간대로 신호등 색깔을 바꾸기 위한 함수들
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