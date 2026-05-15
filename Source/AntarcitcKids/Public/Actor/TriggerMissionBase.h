// TriggerMissionBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Manager/QuestsTypes.h"
#include "TriggerMissionBase.generated.h"


class UBoxComponent;
class UQuestBase;
class UNiagaraSystem;
class ACityVehiclePawn;

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

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mission")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mission")
	UStaticMeshComponent* TriggerBaseBody;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	UBoxComponent* Collision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	UNiagaraSystem* SuccessEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	USceneComponent* NiagaraAnchor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	TSubclassOf<UQuestBase> QuestClass;
	
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
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	

};
