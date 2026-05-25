#pragma once
 
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AgentHudWidget.generated.h"
 
class UAgentDataLogger;
class UAgentLoggerSubsystem;
 
// ─── HUD 표시용 데이터 구조체 ────────────────────────────────────────────────
USTRUCT(BlueprintType)
struct FAgentHudData
{
	GENERATED_BODY()
 
	UPROPERTY(BlueprintReadOnly, Category = "HUD") double SpeedKmh         = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "HUD") double SteeringLeft     = 0.0;  // deg
	UPROPERTY(BlueprintReadOnly, Category = "HUD") double SteeringRight    = 0.0;  // deg
	UPROPERTY(BlueprintReadOnly, Category = "HUD") double AccelerationMps2 = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "HUD") double DecelerationMps2 = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "HUD") double TotalDistanceM   = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "HUD") double UtmEasting       = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "HUD") double UtmNorthing      = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "HUD") int32  UtmZone          = 0;
	UPROPERTY(BlueprintReadOnly, Category = "HUD") float  ElapsedTime      = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "HUD") bool   bIsRecording     = false;
};
 
UCLASS()
class ANTARCITCKIDS_API UAgentHudWidget : public UUserWidget
{
	GENERATED_BODY()
 
protected:
	// ─── UUserWidget 오버라이드 ──────────────────────────────────────────
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
 
public:
	// 블루프린트에서 읽는 최신 데이터 (텍스트 바인딩에 사용)
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	FAgentHudData CurrentData;
 
	// ─── 버튼에서 호출 (블루프린트 버튼 OnClicked에 직접 연결) ──────────
	UFUNCTION(BlueprintCallable, Category = "HUD|Navigation")
	void RestartLevel();
 
	UFUNCTION(BlueprintCallable, Category = "HUD|Navigation")
	void GoToLobby();
 
	// 로비 레벨 이름 – 블루프린트 디테일 패널에서 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD|Navigation")
	FName LobbyLevelName = TEXT("MainMenu");
 
protected:
	// 데이터 갱신 시 블루프린트에서 애니메이션 등 처리 가능
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnDataUpdated(const FAgentHudData& NewData);
 
private:
	// Subsystem 캐시 (NativeConstruct에서 한 번만 가져옴)
	UPROPERTY()
	UAgentLoggerSubsystem* LoggerSubsystem = nullptr;
 
	void PollDataLogger();
};