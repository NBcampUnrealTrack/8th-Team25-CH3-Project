// DigitalTwinGameModeBase.cpp

#include "System/DigitalTwinGameModeBase.h"
#include "System/AntarcitcKidsPlayerController.h"

ADigitalTwinGameModeBase::ADigitalTwinGameModeBase()
{
	DefaultPawnClass = nullptr;  // PlayerController는 빙의할 Pawn 없음
	PlayerControllerClass = AAntarcitcKidsPlayerController::StaticClass();
}
