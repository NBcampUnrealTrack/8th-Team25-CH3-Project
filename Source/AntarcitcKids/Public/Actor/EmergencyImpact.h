// EmergencyImpact.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/TriggerMissionBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EmergencyImpact.generated.h"

class USphereComponent;

UCLASS()
class ANTARCITCKIDS_API AEmergencyImpact : public ATriggerMissionBase
{
	GENERATED_BODY()
	
public:
	AEmergencyImpact();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEmergencyDetected);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEmergencyCleared);
	
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Move")
	UCharacterMovementComponent* MovementComponent;
	
	UPROPERTY(BlueprintAssignable)
	FOnEmergencyDetected OnEmergencyDetected;
	
	UPROPERTY(BlueprintAssignable)
	FOnEmergencyCleared OnEmergencyCleared;
	
	UPROPERTY(VisibleAnywhere,Category = "Impact")
	USphereComponent* ImpactCollision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	USkeletalMeshComponent* CharacterMesh;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* ImpactMontage;
	
	
	UFUNCTION(BlueprintCallable)
	bool IsImpact() { return bIsImpact;}
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Animation")
	float MoveSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Animation")
	float MoveDistance = 500.f;
	
	virtual void OnItemOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult) override;
	
	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION()
	virtual void OnImpactOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult);

	
	
	virtual void BeginPlay() override; 
	
	virtual void Tick(float DeltaSeconds) override;
	
	void PlayImpactMontage();
protected:
	virtual void SetQuestInfo() override;
private:
	FTimerHandle MoveTimerHandle;
	
	bool bIsImpact;
	
	
	FVector StartLocation;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Quest", meta=(AllowPrivateAccess = true))
	FVector DestLocation;
	
	

};
