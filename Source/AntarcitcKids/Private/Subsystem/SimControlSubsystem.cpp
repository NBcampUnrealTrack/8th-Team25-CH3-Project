// SimControlSubsystem.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Subsystem/SimControlSubsystem.h"
#include "Kismet/GameplayStatics.h"

//게임 화면을 정지시키기 위함
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

//게임 시간을 작동시키기 위함
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


//게임 시간 가속
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


//재생과 정지를 하나의 키 입력으로 작동시키기 위한 함수
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
