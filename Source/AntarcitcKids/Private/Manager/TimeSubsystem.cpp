#include "Manager/TimeSubsystem.h"
#include "Engine/DirectionalLight.h"
#include "Kismet/GameplayStatics.h"

void UTimeSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	TArray<AActor*> Results;
	UGameplayStatics::GetAllActorsOfClass(&InWorld, ADirectionalLight::StaticClass(), Results);
	for (AActor* Result : Results)
	{
		if (ADirectionalLight* DirLight = Cast<ADirectionalLight>(Result))
		{
			CachedDirectionalLight = DirLight;
			break;
		}
	}

	LoadFromGameInstance();
	ApplyTimeToLight(CurrentTimeSeconds);
}

void UTimeSubsystem::Tick(float DeltaTime)
{
	CurrentTimeSeconds = FMath::Fmod(CurrentTimeSeconds + DeltaTime * TimeScale, 86400.f);
	ApplyTimeToLight(CurrentTimeSeconds);
	SaveToGameInstance();
	TimeChanged.Broadcast(CurrentTimeSeconds / 86400.f * 360.0, GetCurrentTimeData());
}

void UTimeSubsystem::SetTime(float TimeInSeconds)
{
	CurrentTimeSeconds = FMath::Clamp(TimeInSeconds, 0.f, 86400.f);
	ApplyTimeToLight(CurrentTimeSeconds);
	SaveToGameInstance();
	TimeChanged.Broadcast(CurrentTimeSeconds / 86400.f * 360.0, GetCurrentTimeData());
}

void UTimeSubsystem::ApplyTimeToLight(float TimeInSeconds)
{
	if (!CachedDirectionalLight.IsValid()) return;
	const double Pitch = (TimeInSeconds / 86400.0 * 360.0) - 270.0;
	CachedDirectionalLight->SetActorRotation(FRotator(Pitch, 0.0, 0.0));
}

FTimeOfDay UTimeSubsystem::GetCurrentTimeData() const
{
	FTimeOfDay Data;
	const int32 TotalSeconds = FMath::FloorToInt(CurrentTimeSeconds);
	Data.Hour   = TotalSeconds / 3600;
	Data.Minute = (TotalSeconds % 3600) / 60;
	Data.Second = TotalSeconds % 60;
	Data.Period = CalcTimeOfDay(Data.Hour);
	return Data;
}

ETimeOfDay UTimeSubsystem::CalcTimeOfDay(int32 Hour) const
{
	if (Hour >= 0  && Hour < 6)  return ETimeOfDay::Dawn;
	if (Hour >= 6  && Hour < 17) return ETimeOfDay::Day;
	if (Hour >= 17 && Hour < 20) return ETimeOfDay::Evening;
	return ETimeOfDay::Night;
}

void UTimeSubsystem::SaveToGameInstance()
{
	// Cast<UMyGameInstance>(GetWorld()->GetGameInstance())->SavedTimeSeconds = CurrentTimeSeconds;
}

void UTimeSubsystem::LoadFromGameInstance()
{
	// CurrentTimeSeconds = Cast<UMyGameInstance>(GetWorld()->GetGameInstance())->SavedTimeSeconds;
}