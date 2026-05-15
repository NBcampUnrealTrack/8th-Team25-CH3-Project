// FreeCameraPawn.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FreeCameraPawn.generated.h"

class UInputMappingContext;
class UCameraComponent;
class UFloatingPawnMovement;
class UInputAction;
struct FInputActionValue;

UCLASS()
class ANTARCITCKIDS_API AFreeCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	AFreeCameraPawn();
	
	void BeginPlay() override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FreeCamera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFloatingPawnMovement> FloatingMovement;

	// 움직임 관련
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> FreeCameraMappingContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> LookAction;
	
	//3번 눌렀을 때 입력 연결
	virtual void PossessedBy(AController* NewController) override;

};