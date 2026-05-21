// TriggerMissionBase.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Manager/QuestsTypes.h"
#include "TriggerMissionBase.generated.h"


class UBoxComponent;
class UQuestBase;
class UNiagaraSystem;
class ACityVehiclePawn;
class UCameraComponent;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class AAntarcitcKidsPlayerController;

// 공통 신호: 차량 진입/이탈
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTriggerVehicleEntered, 
	ATriggerMissionBase*, Trigger, ACityVehiclePawn*, Vehicle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTriggerVehicleExited, 
	ATriggerMissionBase*, Trigger, ACityVehiclePawn*, Vehicle);




UCLASS()
class ANTARCITCKIDS_API ATriggerMissionBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ATriggerMissionBase();
	

	
	
	UFUNCTION()
	virtual void OnAreaOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnAreaEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mission")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mission")
	UStaticMeshComponent* TriggerBaseBody;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	UBoxComponent* Collision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	UBoxComponent* AreaCollision;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "CCTV")
	USceneCaptureComponent2D* SceneTarget;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "CCTV")
	UTextureRenderTarget2D* RenderTarget2D;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	UNiagaraSystem* SuccessEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	USceneComponent* NiagaraAnchor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	TSubclassOf<UQuestBase> QuestClass;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Quest")
	bool bIsPawnOrActor;
	
	UPROPERTY()
	UQuestBase* Quest;
	
	UPROPERTY(BlueprintAssignable, Category = "Trigger|Events")
	FOnTriggerVehicleEntered OnVehicleEntered;
	
	UPROPERTY(BlueprintAssignable, Category = "Trigger|Events")
	FOnTriggerVehicleExited OnVehicleExited;
	
	
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
	virtual void TurnOnQuestCamera();
	virtual void TurnOffQuestCamera();
	
	virtual void FocusOn();
	virtual void ResetFocus();
	UTextureRenderTarget2D* GetRenderTarget2D() { return RenderTarget2D;}
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	ACityVehiclePawn* VehiclePawn;
	AAntarcitcKidsPlayerController* PlayerController;
	

};
