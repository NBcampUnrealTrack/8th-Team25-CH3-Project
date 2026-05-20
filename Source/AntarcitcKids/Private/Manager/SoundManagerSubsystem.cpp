#include "Manager/SoundManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"

// 서브시스템 생명주기
void USoundManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
 
	MasterVolume  = 1.0f;
	AmbientVolume = 1.0f;
	VoiceVolume   = 1.0f;
	SystemVolume  = 1.0f;
}
 
void USoundManagerSubsystem::Deinitialize()
{
	StopAllAmbientSounds(0.0f);
	StopVoiceGuidance(0.0f);
 
	Super::Deinitialize();
}

// 환경음 (Ambient)
void USoundManagerSubsystem::PlayAmbientSound(FName Name, USoundBase* Sound, float FadeTime)
{
	if (!Sound) return;
 
	if (AmbientComponents.Contains(Name))
	{
		UAudioComponent* Existing = AmbientComponents[Name];
		if (Existing && Existing->IsPlaying()) return;
 
		// 재생이 끝난 stale 컴포넌트 교체
		AmbientComponents.Remove(Name);
	}
 
	// VolumeMultiplier 1.0f로 스폰 → SetVolumeMultiplier로 볼륨 관리
	// (SpawnSound2D VolumeMultiplier와 FadeIn 목표 볼륨 중복 곱셈 방지)
	UAudioComponent* AudioComp = UGameplayStatics::SpawnSound2D(GetWorld(), Sound, 1.0f);
	if (!AudioComp) return;
 
	AudioComp->SetVolumeMultiplier(GetEffectiveAmbientVolume());
	AudioComp->FadeIn(FadeTime, 1.0f);
	AmbientComponents.Add(Name, AudioComp);
}
 
void USoundManagerSubsystem::StopAmbientSound(FName Name, float FadeTime)
{
	if (!AmbientComponents.Contains(Name)) return;
 
	UAudioComponent* Comp = AmbientComponents[Name];
	if (Comp)
	{
		if (FadeTime > 0.0f)
		{
			// OnAudioFinished 는 Dynamic Multicast Delegate → AddDynamic + UFUNCTION 사용
			// 어떤 컴포넌트가 끝났는지 파라미터가 없으므로 PendingRemoveNames로 이름 관리
			PendingRemoveNames.AddUnique(Name);
			Comp->OnAudioFinished.AddDynamic(this, &USoundManagerSubsystem::OnAmbientFadeOutFinished);
			Comp->FadeOut(FadeTime, 0.0f);
		}
		else
		{
			Comp->Stop();
			AmbientComponents.Remove(Name);
		}
	}
	else
	{
		AmbientComponents.Remove(Name);
	}
}
 
void USoundManagerSubsystem::OnAmbientFadeOutFinished()
{
	// PendingRemoveNames에 쌓인 이름을 순서대로 제거
	if (PendingRemoveNames.Num() > 0)
	{
		FName Name = PendingRemoveNames[0];
		PendingRemoveNames.RemoveAt(0);
		AmbientComponents.Remove(Name);
	}
}
 
void USoundManagerSubsystem::StopAllAmbientSounds(float FadeTime)
{
	// 키 목록 복사 후 순회 (Stop 내부에서 맵이 수정될 수 있으므로)
	TArray<FName> Names;
	AmbientComponents.GetKeys(Names);
 
	for (const FName& Name : Names)
	{
		StopAmbientSound(Name, FadeTime);
	}
}
 
void USoundManagerSubsystem::CleanupFinishedAmbientComponents()
{
	TArray<FName> ToRemove;
	for (auto& Pair : AmbientComponents)
	{
		if (!Pair.Value || !Pair.Value->IsPlaying())
		{
			ToRemove.Add(Pair.Key);
		}
	}
	for (const FName& Key : ToRemove)
	{
		AmbientComponents.Remove(Key);
	}
}

// 시스템 / UI 사운드
void USoundManagerSubsystem::PlaySystemSound(USoundBase* Sound, float Volume)
{
	if (!Sound) return;
 
	UGameplayStatics::PlaySound2D(GetWorld(), Sound, Volume * GetEffectiveSystemVolume());
}

// 음성 가이드
void USoundManagerSubsystem::PlayVoiceGuidance(USoundBase* VoiceSound)
{
	if (!VoiceSound) return;
 
	if (VoiceComponent && VoiceComponent->IsPlaying())
	{
		VoiceComponent->Stop();
	}
 
	VoiceComponent = UGameplayStatics::SpawnSound2D(GetWorld(), VoiceSound, 1.0f);
	if (VoiceComponent)
	{
		VoiceComponent->SetVolumeMultiplier(GetEffectiveVoiceVolume());
	}
}
 
void USoundManagerSubsystem::StopVoiceGuidance(float FadeTime)
{
	if (!VoiceComponent) return;
 
	if (FadeTime > 0.0f)
	{
		VoiceComponent->FadeOut(FadeTime, 0.0f);
	}
	else
	{
		VoiceComponent->Stop();
	}
 
	VoiceComponent = nullptr;
}

// 월드 위치 기반 사운드
void USoundManagerSubsystem::PlaySoundAtLocation(USoundBase* Sound, FVector Location, float Volume)
{
	if (!Sound) return;
 
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location, Volume * MasterVolume);
}

// 볼륨 제어
void USoundManagerSubsystem::SetMasterVolume(float Volume)
{
	MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
 
	for (auto& Pair : AmbientComponents)
	{
		if (Pair.Value)
		{
			Pair.Value->SetVolumeMultiplier(GetEffectiveAmbientVolume());
		}
	}
 
	if (VoiceComponent)
	{
		VoiceComponent->SetVolumeMultiplier(GetEffectiveVoiceVolume());
	}
}
 
void USoundManagerSubsystem::SetAmbientVolume(float Volume)
{
	AmbientVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
 
	for (auto& Pair : AmbientComponents)
	{
		if (Pair.Value)
		{
			Pair.Value->SetVolumeMultiplier(GetEffectiveAmbientVolume());
		}
	}
}
 
void USoundManagerSubsystem::SetVoiceVolume(float Volume)
{
	VoiceVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
 
	if (VoiceComponent)
	{
		VoiceComponent->SetVolumeMultiplier(GetEffectiveVoiceVolume());
	}
}
 
void USoundManagerSubsystem::SetSystemVolume(float Volume)
{
	SystemVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	// PlaySound2D는 단발성이라 소급 적용 불필요
}