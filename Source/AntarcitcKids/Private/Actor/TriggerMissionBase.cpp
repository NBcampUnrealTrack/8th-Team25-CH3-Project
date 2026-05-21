// TriggerMissionBase.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Actor/TriggerMissionBase.h"
#include "Components/BoxComponent.h"
#include "Manager/QuestBase.h"
#include "CityVehiclePawn.h"
#include "Camera/CameraComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetMathLibrary.h"
#include "System/AntarcitcKidsPlayerController.h"

ATriggerMissionBase::ATriggerMissionBase()
{
 	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

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
	
	SceneTarget = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneTarget"));
	SceneTarget->SetupAttachment(SceneRoot);
	SceneTarget->bCaptureEveryFrame = false;
	SceneTarget->bCaptureOnMovement = false;

	

	
	NiagaraAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("Anchor"));
	NiagaraAnchor->SetupAttachment(SceneRoot);
	
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ATriggerMissionBase::OnItemOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ATriggerMissionBase::OnItemEndOverlap);
	
	
	SceneTarget->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

}

void ATriggerMissionBase::BeginPlay()
{
	Super::BeginPlay();
	SetQuestInfo();
	
	UClass* ClassToUse = QuestClass ? QuestClass.Get() : UQuestBase::StaticClass();
	Quest = NewObject<UQuestBase>(this, ClassToUse);
	Quest->OnInitialized(QuestInfo);
	Quest->SetEffect(SuccessEffect);
	
	//렌더 타겟은 CreateSubObject를 통해서 생성하여 UpdateResource를 할 경우, GPU 리소스가 할당되기 전이라서 검은 화면만이 나오게 된다.
	RenderTarget2D = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget2D->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
	RenderTarget2D->InitAutoFormat(1280,720);
	RenderTarget2D->UpdateResourceImmediate(true);
	
	SceneTarget->TextureTarget = RenderTarget2D;
	
	
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
	
	if (ACityVehiclePawn* Vehicle = Cast<ACityVehiclePawn>(OtherActor))
	{
		VehiclePawn = Vehicle;
		PlayerController = Cast<AAntarcitcKidsPlayerController>(GetWorld()->GetFirstPlayerController());
		TurnOnQuestCamera();
	}
}

void ATriggerMissionBase::OnAreaEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsValid(OtherActor)) return;
	
	if (ACityVehiclePawn* Vehicle = Cast<ACityVehiclePawn>(OtherActor))
	{
		VehiclePawn = nullptr;
		PlayerController = nullptr;
		TurnOffQuestCamera();
		
	}
}

void ATriggerMissionBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*UE_LOG(LogTemp,Warning, TEXT("Tick 활성화"));*/
	if (IsValid(VehiclePawn))
	{
		
		FocusOn();
		
	}
}

void ATriggerMissionBase::SetQuestInfo()
{
	if (NiagaraAnchor)
	{
		QuestInfo.QuestLocation = NiagaraAnchor->GetComponentLocation();	
	}
}

void ATriggerMissionBase::TurnOnQuestCamera()
{
	SetActorTickEnabled(true);
		SceneTarget->bCaptureEveryFrame = true;
		SceneTarget->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
		if (AAntarcitcKidsPlayerController* PC = 
		Cast<AAntarcitcKidsPlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			if (bIsPawnOrActor)
				PC->TurnOnQuestCameraView(RenderTarget2D);
		}
		

		
}

void ATriggerMissionBase::TurnOffQuestCamera()
{
	SetActorTickEnabled(false);
	SceneTarget->bCaptureEveryFrame = false;
		
	if (AAntarcitcKidsPlayerController* PC = 
	Cast<AAntarcitcKidsPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		if (bIsPawnOrActor)
		{
			PC->TurnOffQuestCameraView(RenderTarget2D);	
		}
		else
		{
			PC->ResetHightlight();
		}
		
		
	}
}

void ATriggerMissionBase::FocusOn()
{
	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(
	SceneTarget->GetComponentLocation(), 
	VehiclePawn->GetActorLocation());
	/*UE_LOG(LogTemp,Warning, TEXT("LookAt 활성화"));*/
	SceneTarget->SetWorldRotation(LookAt);
		
	float Distance = FVector::Dist(
		SceneTarget->GetComponentLocation(), 
		VehiclePawn->GetActorLocation());
		

		
	SceneTarget->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
	SceneTarget->PostProcessSettings.DepthOfFieldFocalDistance = Distance;
	
	if (!bIsPawnOrActor)
		PlayerController->HighLightActor(SceneTarget);
}

void ATriggerMissionBase::ResetFocus()
{
	
}



