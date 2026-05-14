#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Manager/QuestsTypes.h"
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
class ANTARCITCKIDS_API ATrafficLightActor : public AActor
{
	GENERATED_BODY()

public:
	ATrafficLightActor();

protected:
	virtual void BeginPlay() override;
	
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traffic Light")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traffic Light")
	UStaticMeshComponent* TrafficLightBody;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traffic Light")
	UStaticMeshComponent* RedLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traffic Light")
	UStaticMeshComponent* YellowLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traffic Light")
	UStaticMeshComponent* GreenLight;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	UBoxComponent* Collision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	UNiagaraSystem* SuccessEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	USceneComponent* NiagaraAnchor;
	
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traffic Light")
	float RedDuration = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traffic Light")
	float YellowDuration = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traffic Light")
	float GreenDuration = 5.0f;

	UTrafficLightQuest* Quest;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Quest")
	FQuestInfo TrafficLightQuestInfo;
private:
	FTimerHandle TrafficLightTimerHandle;

	ETrafficLightState CurrentState;

	void SetTrafficLightState(ETrafficLightState NewState);
	void SetQuestInfo();
	void SwitchToRed();
	void SwitchToYellow();
	void SwitchToGreen();
	
	

};