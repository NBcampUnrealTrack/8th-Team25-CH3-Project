// EmergencyImpact.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Actor/EmergencyImpact.h"

#include "AITypes.h"
#include "Components/SphereComponent.h"
#include "Manager/QuestBase.h"
#include "CityVehiclePawn.h"
#include "Components/BoxComponent.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"



AEmergencyImpact::AEmergencyImpact()
{
	bIsImpact = false;
	
	MovementComponent = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->MaxWalkSpeed = MoveSpeed;
	MovementComponent->GravityScale = 1.f;
	
	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	CharacterMesh->SetupAttachment(SceneRoot);
	
	ImpactCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ImpactCollision"));
	ImpactCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ImpactCollision->SetupAttachment(SceneRoot);
	ImpactCollision->SetGenerateOverlapEvents(true);
	
	ImpactCollision->OnComponentBeginOverlap.AddDynamic(this, &AEmergencyImpact::OnImpactOverlap);
	
	AreaCollision->OnComponentBeginOverlap.AddDynamic(this, &ATriggerMissionBase::OnAreaOverlap);
	AreaCollision->OnComponentEndOverlap.AddDynamic(this, &ATriggerMissionBase::OnAreaEndOverlap);
}

void AEmergencyImpact::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnItemOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (ACityVehiclePawn* Vehicle = Cast<ACityVehiclePawn>(OtherActor))
	{
		OnEmergencyDetected.Broadcast();
		SetActorTickEnabled(true);
		MoveSpeed = 300.f;
		
	}
}

void AEmergencyImpact::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnItemEndOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
	
	if (!IsValid(OtherActor)) return;
	
	if (ACityVehiclePawn* Vehicle = Cast<ACityVehiclePawn>(OtherActor))
	{
		if (bIsImpact)
		{
			if (IsValid(Quest) && Quest->IsQuestEnd())
				Quest->OnFailed();
		}
		else
		{
			if (IsValid(Quest) && Quest->IsQuestEnd())
				Quest->OnSuccess();
		}
		bIsImpact = false;

		OnEmergencyCleared.Broadcast();
	}
}

void AEmergencyImpact::OnImpactOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACityVehiclePawn* Vehicle = Cast<ACityVehiclePawn>(OtherActor))
	{
		UE_LOG(LogTemp,Warning,TEXT("OnImpactOverlap 적용"));
		bIsImpact = true;
		MoveSpeed = 0.f;
		PlayImpactMontage();
	}
}



void AEmergencyImpact::BeginPlay()
{
	Super::BeginPlay();
	
	SetQuestInfo();
	Quest->OnInitialized(QuestInfo);
	SetActorTickEnabled(false);
	StartLocation = GetActorLocation();
	MoveSpeed = 0.f;
	

}

void AEmergencyImpact::SetQuestInfo()
{
	Super::SetQuestInfo();
	QuestInfo.QuestType = EQuestClass::EmergencyImpact;
	QuestInfo.QuestName = TEXT("긴급 정지");
	QuestInfo.Description = TEXT("즉시 정지하십시오!");
	
}

void AEmergencyImpact::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	DestLocation = StartLocation + GetActorForwardVector() * MoveDistance;
	
	FVector NewLocation = FMath::VInterpConstantTo(
		GetActorLocation(), DestLocation, DeltaSeconds, MoveSpeed);
	
	SetActorLocation(NewLocation);

	if (FVector::Dist(GetActorLocation(), DestLocation) < 1.f)
	{
		/*UE_LOG(LogTemp,Warning,TEXT("길이 일정 이하라 MoveSpeed 0 "));*/
		MoveSpeed = 0.f;
	}
}

void AEmergencyImpact::PlayImpactMontage()
{
	UE_LOG(LogTemp,Warning,TEXT("PlayImpactMontage 적용"));
	UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance();
	if (!AnimInstance) return;
	
	AddActorLocalRotation(FRotator(0,180,0));
	if (!AnimInstance->Montage_IsPlaying(ImpactMontage))
	{
		UE_LOG(LogTemp,Warning,TEXT("ImapctMontage 적용"));
		float MontageLength = AnimInstance->Montage_Play(ImpactMontage);
		
		FTimerHandle DelayTimer;
		GetWorld()->GetTimerManager().SetTimer(
				DelayTimer,
				FTimerDelegate::CreateWeakLambda(this, [this]()
				{
					this->MoveSpeed = 300.0f;
					this->AddActorLocalRotation(FRotator(0,-180,0));
					this->SetActorTickEnabled(true);
				}),
				MontageLength, 
				false
			);
	}
	


}









