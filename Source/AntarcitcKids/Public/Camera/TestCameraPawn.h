// TestCameraPawn.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TestCameraPawn.generated.h"

class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class AFreeCameraPawn;

UCLASS()
class ANTARCITCKIDS_API ATestCameraPawn : public APawn
{
	GENERATED_BODY()

public:

	ATestCameraPawn();
	
	virtual void BeginPlay() override;
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<USpringArmComponent> ThirdSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<UCameraComponent> ThirdCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<USpringArmComponent> FirstSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<UCameraComponent> FirstCamera;
	
	UPROPERTY(EditAnywhere, Category="Camera")
	TSubclassOf<AFreeCameraPawn> FreeCameraPawnClass;
	
	// 카메라 변경 키 입력
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> FirstCameraAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ThirdCameraAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> FreeCameraAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> CameraMappingContext;
	
private:
	void SetFirstPersonCamera();
	void SetThirdPersonCamera();
	void SetFreeCamera();

	bool bIsThirdPerson = true;
	
	
private:
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	

};
