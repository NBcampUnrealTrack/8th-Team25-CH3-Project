#pragma once
 
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AgentLoggerSubsystem.generated.h"
 
class UAgentDataLogger;
 
UCLASS()
class ANTARCITCKIDS_API UAgentLoggerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
 
public:
	// AgentDataLogger::BeginPlay() 에서 호출
	UFUNCTION(BlueprintCallable, Category = "AgentLogger")
	void RegisterLogger(UAgentDataLogger* InLogger);
 
	// AgentDataLogger::EndPlay() 에서 호출
	UFUNCTION(BlueprintCallable, Category = "AgentLogger")
	void UnregisterLogger(UAgentDataLogger* InLogger);
 
	// 위젯/다른 시스템에서 데이터 접근 시 사용
	UFUNCTION(BlueprintPure, Category = "AgentLogger")
	UAgentDataLogger* GetLogger() const { return ActiveLogger.Get(); }
 
private:
	// WeakObjectPtr : DataLogger가 삭제돼도 크래시 없이 nullptr 반환
	TWeakObjectPtr<UAgentDataLogger> ActiveLogger;
};
