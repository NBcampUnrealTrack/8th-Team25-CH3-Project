// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/SimControlSubsystem.h"
#include "Kismet/GameplayStatics.h"

void USimControlSubsystem::Pause()
{
	if (CurrentState == ESimulationState::Pause) return;
	
	CurrentState = ESimulationState::Pause;
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (IsValid(PlayerController))
	{
		PlayerController->SetPause(true); // true 여야 정지
	}
}

void USimControlSubsystem::Play()
{
	if (CurrentState == ESimulationState::Play) return;
	
	CurrentState = ESimulationState::Play;
	
	CurrentTimeDliation = 1.f;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(),CurrentTimeDliation);
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (IsValid(PlayerController))
	{
		PlayerController->SetPause(false); // false 여야 재생
	}
}

void USimControlSubsystem::SetAccelation(float TimeDliation)
{
	if (CurrentState == ESimulationState::Accelation) return;
	CurrentState = ESimulationState::Accelation;
	
	
	if (TimeDliation > 3.f)
	{
		UE_LOG(LogTemp,Warning,TEXT("CurrentTimeDliation은 3을 넘을 수 없습니다."));
	}
	CurrentTimeDliation = FMath::Clamp(TimeDliation,1.f,3.f);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(),CurrentTimeDliation);
}

void USimControlSubsystem::TogglePlayPause()
{
	if (CurrentState == ESimulationState::Pause)
	{
		Play();
	}
	else if (CurrentState == ESimulationState::Play)
	{
		Pause();
	}
}
