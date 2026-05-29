// AgentLoggerSubsystem.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "DataLogger/AgentLoggerSubsystem.h"
#include "DataLogger/AgentDataLogger.h"
 
void UAgentLoggerSubsystem::RegisterLogger(UAgentDataLogger* InLogger)
{
	if (InLogger)
	{
		ActiveLogger = InLogger;
		UE_LOG(LogTemp, Log, TEXT("[AgentLoggerSubsystem] DataLogger 등록 완료: %s"), *InLogger->GetOwner()->GetName());
	}
}
 
void UAgentLoggerSubsystem::UnregisterLogger(UAgentDataLogger* InLogger)
{
	// 등록된 로거와 동일한 경우에만 해제
	if (ActiveLogger.Get() == InLogger)
	{
		ActiveLogger = nullptr;
		UE_LOG(LogTemp, Log, TEXT("[AgentLoggerSubsystem] DataLogger 해제 완료"));
	}
}
 