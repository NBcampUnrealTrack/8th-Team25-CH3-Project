#include "DataLogger/AgentHudWidget.h"
#include "DataLogger/AgentLoggerSubsystem.h"
#include "DataLogger/AgentDataLogger.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
 
void UAgentHudWidget::NativeConstruct()
{
	Super::NativeConstruct();
 
	// GameInstance Subsystem 캐시 – 이후 NativeTick에서 매번 Get() 안해도 됨
	if (const UGameInstance* GI = GetGameInstance())
	{
		LoggerSubsystem = GI->GetSubsystem<UAgentLoggerSubsystem>();
	}
}
 
void UAgentHudWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
 
	PollDataLogger();
}
 
void UAgentHudWidget::PollDataLogger()
{
	if (!LoggerSubsystem) return;
 
	UAgentDataLogger* Logger = LoggerSubsystem->GetLogger();
	if (!Logger || !Logger->IsRecording())
	{
		CurrentData.bIsRecording = false;
		OnDataUpdated(CurrentData);
		return;
	}
 
	// ── Subsystem에서 꺼낸 Logger로 데이터 갱신 ──────────────────────────
	CurrentData.SpeedKmh         = Logger->GetCurrentSpeedKmh();
	CurrentData.SteeringLeft     = Logger->GetSteeringLeftValue();
	CurrentData.SteeringRight    = Logger->GetSteeringRightValue();
	CurrentData.AccelerationMps2 = Logger->GetAccelerationsMps2();
	CurrentData.DecelerationMps2 = Logger->GetDecelerationMps2();
	CurrentData.TotalDistanceM   = Logger->GetTotalDistanceM();
	CurrentData.UtmEasting       = Logger->GetCurrentUtmEasting();
	CurrentData.UtmNorthing      = Logger->GetCurrentUtmNorthing();
	CurrentData.UtmZone          = Logger->GetOriginUtmZone();
	CurrentData.ElapsedTime      = Logger->GetElapsedRecordingTime();
	CurrentData.bIsRecording     = true;
 
	// 블루프린트 이벤트 → WBP에서 텍스트/애니메이션 갱신
	OnDataUpdated(CurrentData);
}
 
// ─── 버튼 이벤트 ─────────────────────────────────────────────────────────────
 
void UAgentHudWidget::QuitGame()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
}
 
void UAgentHudWidget::GoToLobby()
{
	if (!LobbyLevelName.IsNone())
	{
		UGameplayStatics::OpenLevel(this, LobbyLevelName);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[AgentHudWidget] LobbyLevelName이 비어 있습니다."));
	}
}