// TrafficLightActor.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TriggerMissionBase.h"
#include "GameFramework/Actor.h"
#include "Quest/QuestsTypes.h"
#include "TrafficLightActor.generated.h"

class UBoxComponent;
class UTrafficLightQuest;
class UNiagaraSystem;

UENUM(BlueprintType)
enum class ETrafficLightState : uint8
{
	Red,
	Yellow,
	Green
};

UCLASS()
class ANTARCITCKIDS_API ATrafficLightActor : public ATriggerMissionBase
{
	GENERATED_BODY()

public:
	ATrafficLightActor();
	//퀘스트 영역을 확인하기 위한 델리게이트
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrafficLightStateChanged, ETrafficLightState, NewState);
	
	UPROPERTY(BlueprintAssignable)
	FOnTrafficLightStateChanged OnStateChanged;
	
protected:
	virtual void BeginPlay() override;
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traffic Light")
	UStaticMeshComponent* RedLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traffic Light")
	UStaticMeshComponent* YellowLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traffic Light")
	UStaticMeshComponent* GreenLight;
	
	virtual void OnAreaOverlap(
		UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult) override;
	
	virtual void OnAreaEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;
	
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traffic Light")
	float RedDuration = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traffic Light")
	float YellowDuration = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traffic Light")
	float GreenDuration = 5.0f;

	
	void SetQuestInfo() override;
private:
	FTimerHandle TrafficLightTimerHandle;

	ETrafficLightState CurrentState;
	
	bool IsVehicleInArea;

	void SetTrafficLightState(ETrafficLightState NewState);

	void SwitchToRed();
	void SwitchToYellow();
	void SwitchToGreen();
	
	

};