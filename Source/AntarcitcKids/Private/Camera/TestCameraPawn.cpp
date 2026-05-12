// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/TestCameraPawn.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/FreeCameraPawn.h"


// Sets default values
ATestCameraPawn::ATestCameraPawn()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	// 3인칭 카메라용 스프링암
	ThirdSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdSpringArm"));
	ThirdSpringArm->SetupAttachment(RootComponent);
	ThirdSpringArm->TargetArmLength = 300.f;
	ThirdSpringArm->SetRelativeLocation(FVector(0.f, 0.f, 80.f));

	// 3인칭 카메라
	ThirdCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdCamera"));
	ThirdCamera->SetupAttachment(ThirdSpringArm);
	
	// 1인칭 카메라용 스프링암
	FirstSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FirstSpringArm"));
	FirstSpringArm->SetupAttachment(RootComponent);
	FirstSpringArm->TargetArmLength = 0.f;

	// 1인칭 카메라
	FirstCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstCamera"));
	FirstCamera->SetupAttachment(FirstSpringArm);

}

void ATestCameraPawn::BeginPlay()
{
	Super::BeginPlay();

	SetThirdPersonCamera();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController)
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->AddMappingContext(CameraMappingContext, 0);
			}
		}
	}
}

void ATestCameraPawn::SetFirstPersonCamera()
{
	FirstCamera->SetActive(true);
	ThirdCamera->SetActive(false);
}

void ATestCameraPawn::SetThirdPersonCamera()
{
	FirstCamera->SetActive(false);
	ThirdCamera->SetActive(true);
}

void ATestCameraPawn::SetFreeCamera()
{
	FirstCamera->SetActive(false);
	ThirdCamera->SetActive(false);
	
	APlayerController* PC = Cast<APlayerController>(GetController());

	if (!PC) return;

	FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 200.f);
	FRotator SpawnRotation = GetActorRotation();

	AFreeCameraPawn* FreePawn = GetWorld()->SpawnActor<AFreeCameraPawn>(
	FreeCameraPawnClass,
	SpawnLocation,
	SpawnRotation
	);

	if (FreePawn)
	{
		PC->Possess(FreePawn);
	}
	
}



void ATestCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(
			FirstCameraAction,
			ETriggerEvent::Started,
			this,
			&ATestCameraPawn::SetFirstPersonCamera
		);

		EnhancedInputComponent->BindAction(
			ThirdCameraAction,
			ETriggerEvent::Started,
			this,
			&ATestCameraPawn::SetThirdPersonCamera
		);

		EnhancedInputComponent->BindAction(
			FreeCameraAction,
			ETriggerEvent::Started,
			this,
			&ATestCameraPawn::SetFreeCamera
		);
	}
}

