// DigitalTwinGameModeBase.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "System/DigitalTwinGameModeBase.h"
#include "System/AntarcitcKidsPlayerController.h"
#include "MoviePlayer.h"
#include "Manager/DigitalTwinConfiguration.h"

ADigitalTwinGameModeBase::ADigitalTwinGameModeBase()
{
	DefaultPawnClass = nullptr;  // PlayerController는 빙의할 Pawn 없음
	PlayerControllerClass = AAntarcitcKidsPlayerController::StaticClass();
}

void ADigitalTwinGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle WaitForLoadHandle;
	
	// 모든 Begin Play가 끝난 뒤 한 프레임 뒤에 로딩 종료
	// 0.0f + false = 다음 프레임 1회 실행
	GetWorldTimerManager().SetTimer(
		WaitForLoadHandle, 
		[this](){
			UDigitalTwinConfiguration* GI = Cast<UDigitalTwinConfiguration>(GetGameInstance());
			if (GI)
			{
				GI->NotifyLevelReady();
			}
		}, 0.0f, false);
	
}
