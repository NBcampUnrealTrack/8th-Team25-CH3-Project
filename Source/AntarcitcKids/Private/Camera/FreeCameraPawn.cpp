
#include "Camera/FreeCameraPawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"

AFreeCameraPawn::AFreeCameraPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	FreeCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FreeCamera"));
	FreeCamera->SetupAttachment(SceneRoot);
	FreeCamera->SetActive(true);
	FreeCamera->bUsePawnControlRotation = true;

	FloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingMovement"));
	
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = false;

	
}

void AFreeCameraPawn::BeginPlay()
{
	Super::BeginPlay();
}


void AFreeCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFreeCameraPawn::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFreeCameraPawn::Look);
	}
}

void AFreeCameraPawn::Move(const FInputActionValue& Value)
{
	const FVector2D MoveValue = Value.Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(), MoveValue.X);
	AddMovementInput(GetActorRightVector(), MoveValue.Y);
}

void AFreeCameraPawn::Look(const FInputActionValue& Value)
{
	const FVector2D LookValue = Value.Get<FVector2D>();

	AddControllerYawInput(LookValue.X);
	AddControllerPitchInput(LookValue.Y);
}

void AFreeCameraPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	APlayerController* PC = Cast<APlayerController>(NewController);
	if (!PC) return;

	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!LocalPlayer) return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (!Subsystem) return;

	Subsystem->AddMappingContext(FreeCameraMappingContext, 0);
}

