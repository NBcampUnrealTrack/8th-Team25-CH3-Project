// TriggerMissionBase.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Actor/TriggerMissionBase.h"
#include "Components/BoxComponent.h"
#include "Manager/QuestBase.h"
#include "CityVehiclePawn.h"
#include "Camera/CameraComponent.h"

ATriggerMissionBase::ATriggerMissionBase()
{
 	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	TriggerBaseBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerBaseBody"));
	TriggerBaseBody->SetupAttachment(SceneRoot);
	
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(SceneRoot);
	Collision->SetGenerateOverlapEvents(true);
	
	AreaCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("AreaCollision"));
	AreaCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	AreaCollision->SetupAttachment(SceneRoot);
	AreaCollision->SetGenerateOverlapEvents(true);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SceneRoot);
	
	
	NiagaraAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("Anchor"));
	NiagaraAnchor->SetupAttachment(SceneRoot);
	
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ATriggerMissionBase::OnItemOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ATriggerMissionBase::OnItemEndOverlap);
}

void ATriggerMissionBase::BeginPlay()
{
	Super::BeginPlay();
	SetQuestInfo();
	
	UClass* ClassToUse = QuestClass ? QuestClass.Get() : UQuestBase::StaticClass();
	Quest = NewObject<UQuestBase>(this, ClassToUse);
	
	Quest->OnInitialized(QuestInfo);
	Quest->SetEffect(SuccessEffect);
	
}

void ATriggerMissionBase::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACityVehiclePawn* Vehicle = Cast<ACityVehiclePawn>(OtherActor))
	{
		OnVehicleEntered.Broadcast(this, Vehicle);
		
	}
}

void ATriggerMissionBase::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACityVehiclePawn* Vehicle = Cast<ACityVehiclePawn>(OtherActor))
	{
		OnVehicleExited.Broadcast(this, Vehicle);
	}
}

void ATriggerMissionBase::OnAreaOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ATriggerMissionBase::OnAreaEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

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

