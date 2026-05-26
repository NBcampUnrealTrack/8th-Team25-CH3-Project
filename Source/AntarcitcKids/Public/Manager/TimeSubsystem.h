#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TimeSubsystem.generated.h"

UENUM(BlueprintType)
enum class ETimeOfDay : uint8
{
    Dawn    UMETA(DisplayName = "Dawn"),
    Day     UMETA(DisplayName = "Day"),
    Evening UMETA(DisplayName = "Evening"),
    Night   UMETA(DisplayName = "Night"),
};

USTRUCT(BlueprintType)
struct FTimeOfDay
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly) int32 Hour   = 0;
    UPROPERTY(BlueprintReadOnly) int32 Minute = 0;
    UPROPERTY(BlueprintReadOnly) int32 Second = 0;
    UPROPERTY(BlueprintReadOnly) ETimeOfDay Period = ETimeOfDay::Day;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTimeChanged, double, Pitch, FTimeOfDay, TimeData);

class ADirectionalLight;

UCLASS()
class ANTARCITCKIDS_API UTimeSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category = "Time")
    FTimeChanged TimeChanged;

    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UTimeSubsystem, STATGROUP_Tickables); }

    UFUNCTION(BlueprintCallable, Category = "Time")
    void SetTime(float TimeInSeconds);

    UFUNCTION(BlueprintPure, Category = "Time")
    FTimeOfDay GetCurrentTimeData() const;

    UFUNCTION(BlueprintPure, Category = "Time")
    float GetCurrentTimeSeconds() const { return CurrentTimeSeconds; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    float TimeScale = 1.f;

private:
    UPROPERTY()
    TWeakObjectPtr<ADirectionalLight> CachedDirectionalLight;

    float CurrentTimeSeconds = 43200.f;

    void ApplyTimeToLight(float TimeInSeconds);
    ETimeOfDay CalcTimeOfDay(int32 Hour) const;
    void SaveToGameInstance();
    void LoadFromGameInstance();
};