// CameraSceneComponent.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CameraSensorTypes.h"
#include "CameraSceneComponent.generated.h"

class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class UMaterialInterface;
class UMaterialInstanceDynamic;

UCLASS( ClassGroup=(Sensor), meta=(BlueprintSpawnableComponent) )
class ANTARCITCKIDS_API UCameraSceneComponent : public USceneComponent
{
	GENERATED_BODY()
public:
	UCameraSceneComponent();
	
	UTextureRenderTarget2D* GetRednerTarget() const { return RenderTarget; }

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	UFUNCTION(BlueprintCallable, Category = "CameraSensor")
	void ApplyPreset(ECameraSensorPreset NewPreset);
	//"Tesla HW3 Wide", "Waymo" 같은 미리 정의된 카메라 사양을 한 번에 적용.
	
	UFUNCTION(BlueprintCallable, Category = "CameraSensor")
	void SetCaptureRate(float Hz);
	//초당 몇 장 찍을지 설정 (예: 30Hz = 1초에 30장).
	
	UFUNCTION(BlueprintCallable, Category = "CameraSensor")
	void CaptureOnce();

	UFUNCTION(BlueprintCallable, Category = "CameraSensor")
	void RefreshSettings();
	//설정을 갱신.
	
	void InitializeCapture();
	void CreateRenderTarget();
	//CameraSensor의 RenderTarget을 설정(UI 표기 및 저장을 수행)
	void ConfigureSceneCapture();
	//
	void ApplyPostProcessSettings();
	void ApplyLensDistortion();
	
	void StartCaptureTimer();
	void StopCaptureTimer();
	void OnCaptureTimer();

	void SaveCameraImage();
	//RenderTarget 이미지를 디스크에 JPG로 저장.
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|Config",
		meta=(AllowPrivateAccess="true"))
	ECameraSensorPreset Preset = ECameraSensorPreset::TeslaHW3_Wide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|Config",
		meta=(AllowPrivateAccess="true"))
	bool bSensorEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|Intrinsics",
		meta=(AllowPrivateAccess="true"))
	FCameraSensorIntrinsics Intrinsics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|Distortion",
		meta=(AllowPrivateAccess="true"))
	FLensDistortionParams Distortion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|Noise",
		meta=(AllowPrivateAccess="true"))
	FSensorNoiseParams Noise;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|PostProcess",
		meta=(AllowPrivateAccess="true"))
	FCameraPostProcessEffects PostProcess;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|Exposure",
		meta=(AllowPrivateAccess="true"))
	FAutoExposureParams Exposure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|DataSave",
		meta=(AllowPrivateAccess="true"))
	bool bIsDataSaving = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|DataSave",
		meta=(AllowPrivateAccess="true"))
	FSensorDataSaveConfig DataSaveConfig;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSensor|Distortion",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMaterialInterface> LensDistortionMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraSensor|Output",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraSensor|Output",
		meta=(AllowPrivateAccess="true"))
	int64 FrameCount = 0;

	UPROPERTY()
	TObjectPtr<USceneCaptureComponent2D> SceneCapture;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DistortionMID;

	FTimerHandle CaptureTimerHandle;
};
