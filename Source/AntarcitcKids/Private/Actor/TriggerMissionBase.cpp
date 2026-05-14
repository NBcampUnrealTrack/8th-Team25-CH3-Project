// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/TriggerMissionBase.h"
#include "Components/BoxComponent.h"
#include "Manager/QuestBase.h"

// Sets default values
ATriggerMissionBase::ATriggerMissionBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	TriggerBaseBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerBaseBody"));
	TriggerBaseBody->SetupAttachment(SceneRoot);
	
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(SceneRoot);
	Collision->SetGenerateOverlapEvents(true);
	
	NiagaraAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("Anchor"));
	NiagaraAnchor->SetupAttachment(SceneRoot);
	
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ATriggerMissionBase::OnItemOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ATriggerMissionBase::OnItemEndOverlap);
}

// Called when the game starts or when spawned
void ATriggerMissionBase::BeginPlay()
{
	Super::BeginPlay();
	SetQuestInfo();
	
	Quest = NewObject<UQuestBase>(this);
	Quest->OnInitialized(QuestInfo);
	Quest->SetEffect(SuccessEffect);
	
}

void ATriggerMissionBase::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ATriggerMissionBase::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

// Called every frame
void ATriggerMissionBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATriggerMissionBase::SetQuestInfo()
{
	if (NiagaraAnchor)
	{
		QuestInfo.QuestLocation = NiagaraAnchor->GetComponentLocation();	
	}
}

