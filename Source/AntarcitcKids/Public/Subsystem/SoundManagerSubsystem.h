// SoundManagerSubsystem.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SoundManagerSubsystem.generated.h"

UCLASS()
class ANTARCITCKIDS_API USoundManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
 
public:
	// 서브시스템 초기화 / 정리
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
 
	// 날씨 및 환경음 (비, 바람 등 중첩 가능 / 루프 사운드용)
	// param Name: 소리를 식별할 이름 (예: "Rain", "Wind")
	UFUNCTION(BlueprintCallable, Category = "Sound|Ambient")
	void PlayAmbientSound(FName Name, USoundBase* Sound, float FadeTime = 1.0f);
 
	UFUNCTION(BlueprintCallable, Category = "Sound|Ambient")
	void StopAmbientSound(FName Name, float FadeTime = 1.0f);
 
	// 현재 재생 중인 모든 환경음 정지 (레벨 전환 등)
	UFUNCTION(BlueprintCallable, Category = "Sound|Ambient")
	void StopAllAmbientSounds(float FadeTime = 1.0f);
 
	// 시스템 알람 및 UI 클릭 소리 (단발성)
	UFUNCTION(BlueprintCallable, Category = "Sound|UI")
	void PlaySystemSound(USoundBase* Sound, float Volume = 1.0f);
 
	// 네비게이션 및 음성 가이드 (이전 안내 음성을 끊고 최신 안내 재생)
	UFUNCTION(BlueprintCallable, Category = "Sound|Voice")
	void PlayVoiceGuidance(USoundBase* VoiceSound);
 
	UFUNCTION(BlueprintCallable, Category = "Sound|Voice")
	void StopVoiceGuidance(float FadeTime = 0.3f);
 
	// 월드 위치 기반 소리 (차량 센서 알람, 충돌 등)
	UFUNCTION(BlueprintCallable, Category = "Sound|World")
	void PlaySoundAtLocation(USoundBase* Sound, FVector Location, float Volume = 1.0f);
 
	// 마스터 볼륨 설정 (0.0 ~ 1.0)
	UFUNCTION(BlueprintCallable, Category = "Sound")
	void SetMasterVolume(float Volume);
 
	// 카테고리별 볼륨 설정 (0.0 ~ 1.0)
	UFUNCTION(BlueprintCallable, Category = "Sound")
	void SetAmbientVolume(float Volume);
 
	UFUNCTION(BlueprintCallable, Category = "Sound")
	void SetVoiceVolume(float Volume);
 
	UFUNCTION(BlueprintCallable, Category = "Sound")
	void SetSystemVolume(float Volume);
 
	UFUNCTION(BlueprintCallable, Category = "Sound")
	float GetMasterVolume() const { return MasterVolume; }
 
private:
	// 날씨/환경음 중첩 관리를 위한 맵
	UPROPERTY()
	TMap<FName, TObjectPtr<UAudioComponent>> AmbientComponents;
 
	// FadeOut 완료 후 제거할 이름 목록 (AddDynamic 콜백용)
	TArray<FName> PendingRemoveNames;
 
	// 안내 음성 관리용
	UPROPERTY()
	TObjectPtr<UAudioComponent> VoiceComponent;
 
	// 볼륨 설정
	float MasterVolume = 1.0f;
	float AmbientVolume = 1.0f;
	float VoiceVolume   = 1.0f;
	float SystemVolume  = 1.0f;
 
	// 실제 적용 볼륨 계산 헬퍼
	float GetEffectiveAmbientVolume() const { return MasterVolume * AmbientVolume; }
	float GetEffectiveVoiceVolume()   const { return MasterVolume * VoiceVolume; }
	float GetEffectiveSystemVolume()  const { return MasterVolume * SystemVolume; }
 
	// FadeOut 완료 콜백 (OnAudioFinished는 Dynamic Delegate라 UFUNCTION 필요)
	UFUNCTION()
	void OnAmbientFadeOutFinished();
 
	// 재생이 끝난 AmbientComponent 항목 정리
	void CleanupFinishedAmbientComponents();
};