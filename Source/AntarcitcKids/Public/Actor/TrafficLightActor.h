#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrafficLightActor.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traffic Light")
	float RedDuration = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traffic Light")
	float YellowDuration = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traffic Light")
	float GreenDuration = 5.0f;

private:
	FTimerHandle TrafficLightTimerHandle;

	ETrafficLightState CurrentState;

	void SetTrafficLightState(ETrafficLightState NewState);

	void SwitchToRed();
	void SwitchToYellow();
	void SwitchToGreen();
};