// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Manager/QuestsTypes.h"
#include "TriggerMissionBase.generated.h"


class UBoxComponent;
class UQuestBase;
class UNiagaraSystem;

UCLASS()
class ANTARCITCKIDS_API ATriggerMissionBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATriggerMissionBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traffic Light")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traffic Light")
	UStaticMeshComponent* TriggerBaseBody;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	UBoxComponent* Collision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	UNiagaraSystem* SuccessEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	USceneComponent* NiagaraAnchor;
	
	UQuestBase* Quest;
	
	UFUNCTION()
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Quest")
	FQuestInfo QuestInfo;
	
	virtual void SetQuestInfo();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	

};
