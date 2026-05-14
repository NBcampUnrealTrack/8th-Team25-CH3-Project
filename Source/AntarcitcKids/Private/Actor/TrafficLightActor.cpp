#include "Actor/TrafficLightActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
#include "Manager/TrafficLightQuest.h"
#include "NiagaraSystem.h"
#include "Manager/QuestsTypes.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"

ATrafficLightActor::ATrafficLightActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	TrafficLightBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrafficLightBody"));
	TrafficLightBody->SetupAttachment(SceneRoot);

	RedLight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RedLight"));
	RedLight->SetupAttachment(SceneRoot);

	YellowLight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("YellowLight"));
	YellowLight->SetupAttachment(SceneRoot);

	GreenLight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GreenLight"));
	GreenLight->SetupAttachment(SceneRoot);
	
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(SceneRoot);
	Collision->SetGenerateOverlapEvents(true);
	
	NiagaraAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("Anchor"));
	NiagaraAnchor->SetupAttachment(SceneRoot);
	
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ATrafficLightActor::OnItemOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ATrafficLightActor::OnItemEndOverlap);

	

	CurrentState = ETrafficLightState::Red;
}

void ATrafficLightActor::BeginPlay()
{
	Super::BeginPlay();
	
	SwitchToRed();
	
	
	SetQuestInfo();
	UE_LOG(LogTemp,Warning,TEXT("TrafficLightActor Activate"));
	Quest = NewObject<UTrafficLightQuest>(this);
	Quest->OnInitialized(TrafficLightQuestInfo);	
	Quest->SetEffect(SuccessEffect);
	Quest->OnSuccess();

}

void ATrafficLightActor::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		if (CurrentState == ETrafficLightState::Red)
		{
			Quest->OnFailed();
		}
		else
		{
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
		TrafficLightQuestInfo.QuestLocation = NiagaraAnchor->GetComponentLocation();	
	}
	
	TrafficLightQuestInfo.Description = TEXT("신호등 알맞게 진행");
	TrafficLightQuestInfo.QuestName = TEXT("신호등 준수 퀘스트");
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