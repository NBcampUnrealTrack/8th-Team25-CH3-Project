// CameraSceneComponent.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "Sensor/CameraSceneComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "HAL/PlatformFileManager.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "TimerManager.h"
#include "Subsystem/SensorSubSystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogCameraSensor, Log, All);

UCameraSceneComponent::UCameraSceneComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	//카메라 렌더링을 위한 SceneCapture 생성
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SensorSceneCapture"));
	SceneCapture->SetupAttachment(this);
}

void UCameraSceneComponent::OnRegister()
{
	Super::OnRegister();
	//등록
	if (SceneCapture && SceneCapture->IsRegistered() == false)
	{
		//SceneCapture를 부모인 여기에 등록하고 있다.
		SceneCapture->SetupAttachment(this);
		SceneCapture->RegisterComponent();
	}
}

void UCameraSceneComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeCapture();
	ApplyPreset(Preset);
}

void UCameraSceneComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopCaptureTimer();
	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void UCameraSceneComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UCameraSceneComponent, Preset))
	{
		ApplyPreset(Preset);
	}
}
#endif

//초기화
void UCameraSceneComponent::InitializeCapture()
{
	//주인  가져오기
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogCameraSensor, Error, TEXT("CameraSensorComponent has no owning actor."));
		return;
	}
	//렌더 타겟 설정
	CreateRenderTarget();
	//
	ConfigureSceneCapture();
	
	ApplyPostProcessSettings();
	//렌즈 왜곡 효과를 화면에 입힙
	ApplyLensDistortion();
	
	//bSensorEnabled는 기본적으로 true;
	if (bSensorEnabled)
	{
		StartCaptureTimer();
	}

	USensorSubSystem* SensorSubSystem = GetWorld()->GetSubsystem<USensorSubSystem>();
	if (SensorSubSystem)
	{
		SensorSubSystem->OnCameraPresetChanged.AddUObject(this,&UCameraSceneComponent::ApplyPreset);
		SensorSubSystem->OnSetCameraHz.AddUObject(this,&UCameraSceneComponent::SetCaptureRate);
	}
	//카메라 내부 광학 설정 값
	UE_LOG(LogCameraSensor, Log, TEXT("CameraSensor initialized: %dx%d @ %.0f Hz, FOV %.0f°"),
		Intrinsics.ImageWidth, Intrinsics.ImageHeight, Intrinsics.FrameRate, Intrinsics.FOVDegrees);
}

//보통 - GPU - 렌더링 - 화면
//우리 - GPU - 렌더링(해당 자료 필요!)
void UCameraSceneComponent::CreateRenderTarget()
{
	//카메라 렌더된 설정대로 렌더 타겟을 생성한다.
	//RenderTarget : GPU가 렌더링한 결과물을 저장하는 2D 텍스처 버퍼
	//렌더링할 사진의 포맷
	RenderTarget = NewObject<UTextureRenderTarget2D>(this, TEXT("SensorRenderTarget"));
	RenderTarget->InitAutoFormat(Intrinsics.ImageWidth, Intrinsics.ImageHeight);
	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
	//축소 방지
	RenderTarget->bAutoGenerateMips = false;
	//리소스 즉시 업로드
	RenderTarget->UpdateResourceImmediate(true);
}

//SceneCapture는 기본적으로 메인 카메라 이외에 다른 곳의 장면도 캡쳐하는 것
void UCameraSceneComponent::ConfigureSceneCapture()
{
	
	if (SceneCapture == nullptr || RenderTarget == nullptr)
	{
		UE_LOG(LogCameraSensor, Error, TEXT("SceneCaptureComponent is not ready."));
		return;
	}
	//RenderTarget은 저장 장소, SceneCapture는 렌더링하는 곳
	SceneCapture->TextureTarget = RenderTarget;
	SceneCapture->FOVAngle = Intrinsics.FOVDegrees;
	//디지털 트윈의 센서 세팅대로 가야 하기 때문에, 매 프레임, 이동마다의 캡처 비활성화
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bCaptureOnMovement = false;
	//렌더 상태를 계속 유지한다.
	SceneCapture->bAlwaysPersistRenderingState = true;

	//최종 후처리 색상까지 끝낸 후에 나와야 한다.
	SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	//PostProcessing을 100%처리하겠다.
	SceneCapture->PostProcessBlendWeight = 1.0f;
}

//촬영 결과를 후처리하는 세팅
void UCameraSceneComponent::ApplyPostProcessSettings()
{
	if (SceneCapture == nullptr)
	{
		UE_LOG(LogCameraSensor, Error, TEXT("There is no SceneCaptureComponent."));
		return;
	}
	//후처리 세팅을 설정하는 중이다.
	FPostProcessSettings& PP = SceneCapture->PostProcessSettings;

	PP.bOverride_VignetteIntensity = true;
	PP.VignetteIntensity = PostProcess.VignetteIntensity;

	PP.bOverride_BloomIntensity = true;
	PP.BloomIntensity = PostProcess.BloomIntensity;

	PP.bOverride_MotionBlurAmount = true;
	PP.MotionBlurAmount = PostProcess.MotionBlurAmount;

	PP.bOverride_SceneFringeIntensity = true;
	PP.SceneFringeIntensity = PostProcess.ChromaticAberration;

	PP.bOverride_LensFlareIntensity = true;
	PP.LensFlareIntensity = PostProcess.LensFlareIntensity;

	if (Exposure.bEnableAutoExposure)
	{
		PP.bOverride_AutoExposureMethod = true;
		PP.AutoExposureMethod = EAutoExposureMethod::AEM_Histogram;

		PP.bOverride_AutoExposureMinBrightness = true;
		PP.AutoExposureMinBrightness = Exposure.MinEV;

		PP.bOverride_AutoExposureMaxBrightness = true;
		PP.AutoExposureMaxBrightness = Exposure.MaxEV;

		PP.bOverride_AutoExposureSpeedUp = true;
		PP.AutoExposureSpeedUp = Exposure.SpeedUp;

		PP.bOverride_AutoExposureSpeedDown = true;
		PP.AutoExposureSpeedDown = Exposure.SpeedDown;
	}
	else
	{
		PP.bOverride_AutoExposureMethod = true;
		PP.AutoExposureMethod = EAutoExposureMethod::AEM_Manual;
	}
}

//렌즈 왜곡을 적용한다.
void UCameraSceneComponent::ApplyLensDistortion()
{
	if (SceneCapture == nullptr)
	{
		UE_LOG(LogCameraSensor, Error, TEXT("There is no SceneCaptureComponent"));
		return;
	}
	if (Distortion.HasDistortion() == false)
	{
		UE_LOG(LogCameraSensor, Error, TEXT("There is no Distortion"));
		return;
	}
	if (LensDistortionMaterial == nullptr)
	{
		UE_LOG(LogCameraSensor, Error, TEXT("There is no LensDistortionMaterial"));
		return;
	}
	//머터리얼을 특정 요소(UV 왜곡, K1/K2/P1/P2, 화면 회어짐)을 동적으로 적용하기 위한 장치
	//렌즈마다 왜곡값이 다른 것을 이용한다. 
	//PostProcess Material은 렌더된 화면을 다시 변형하기 위한 장치이므로, 많은 센서에 유동적인 대응
	DistortionMID = UMaterialInstanceDynamic::Create(LensDistortionMaterial, this);
	if (DistortionMID == nullptr)
	{
		UE_LOG(LogCameraSensor, Error, TEXT("Failed to create DistortionMID."));
		return;
	}

	DistortionMID->SetScalarParameterValue(TEXT("K1"), Distortion.K1);
	DistortionMID->SetScalarParameterValue(TEXT("K2"), Distortion.K2);
	DistortionMID->SetScalarParameterValue(TEXT("K3"), Distortion.K3);
	DistortionMID->SetScalarParameterValue(TEXT("P1"), Distortion.P1);
	DistortionMID->SetScalarParameterValue(TEXT("P2"), Distortion.P2);

	FWeightedBlendable Blendable;
	Blendable.Object = DistortionMID.Get();
	Blendable.Weight = 1.0f;
	SceneCapture->PostProcessSettings.WeightedBlendables.Array.Add(Blendable);
}

//CaptureTimer를 시작한다.
void UCameraSceneComponent::StartCaptureTimer()
{
	if (!GetWorld()) return;
	//FPS 초당 몇 번
	const float Interval = 1.0f / FMath::Max(Intrinsics.FrameRate, 1.0f);
	GetWorld()->GetTimerManager().SetTimer(
		CaptureTimerHandle, this, &UCameraSceneComponent::OnCaptureTimer,
		Interval, true);
}

void UCameraSceneComponent::StopCaptureTimer()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(CaptureTimerHandle);
	}
}

//실제로 캡처하는 함수
void UCameraSceneComponent::OnCaptureTimer()
{
	if (bSensorEnabled && SceneCapture)
	{
		
		SceneCapture->CaptureScene();
		FrameCount++;

		if (bIsDataSaving && RenderTarget)
		{
			SaveCameraImage();
		}
	}
}

//CaptureRate를 다시 설정한다.
void UCameraSceneComponent::SetCaptureRate(float Hz)
{
	Intrinsics.FrameRate = FMath::Clamp(Hz, 1.0f, 120.0f);
	StopCaptureTimer();
	if (bSensorEnabled)
	{
		StartCaptureTimer();
	}
}

//한 번 캡처한다.
void UCameraSceneComponent::CaptureOnce()
{
	if (SceneCapture)
	{
		SceneCapture->CaptureScene();
		FrameCount++;
	}
}

//캡처의 후처리, 왜곡, 캡처 설정을 다시 설정한다.
void UCameraSceneComponent::RefreshSettings()
{
	if (SceneCapture == nullptr)
	{
		UE_LOG(LogCameraSensor, Error, TEXT("There is no SceneCaptureComponent."));
		return;
	}

	SceneCapture->FOVAngle = Intrinsics.FOVDegrees;

	if (RenderTarget &&
		(RenderTarget->SizeX != Intrinsics.ImageWidth || RenderTarget->SizeY != Intrinsics.ImageHeight))
	{
		RenderTarget->InitAutoFormat(Intrinsics.ImageWidth, Intrinsics.ImageHeight);
		RenderTarget->UpdateResourceImmediate(true);
	}

	SceneCapture->PostProcessSettings.WeightedBlendables.Array.Empty();
	ApplyPostProcessSettings();
	ApplyLensDistortion();

	StopCaptureTimer();
	if (bSensorEnabled)
	{
		StartCaptureTimer();
	}
}

//정해진 프리셋에 따라서 설정, 왜곡 후처리를 직접 설정한다.
void UCameraSceneComponent::ApplyPreset(ECameraSensorPreset NewPreset)
{
	Preset = NewPreset;

	switch (NewPreset)
	{
	case ECameraSensorPreset::TeslaHW3_Wide:
		Intrinsics = { 1280, 960, 36.0f, 120.0f, 1.5f };
		Distortion = { -0.3f, 0.1f, 0.0f, 0.0f, 0.0f };
		PostProcess.VignetteIntensity = 0.4f;
		PostProcess.ChromaticAberration = 0.5f;
		PostProcess.MotionBlurAmount = 0.5f;
		PostProcess.BloomIntensity = 0.0f;
		PostProcess.LensFlareIntensity = 0.1f;
		Exposure = { true, 2.0f, 14.0f, 3.0f, 1.0f };
		Noise = { true, 0.0f, 0.02f, 0.01f };
		break;

	case ECameraSensorPreset::TeslaHW3_Main:
		Intrinsics = { 1280, 960, 36.0f, 50.0f, 5.0f };
		Distortion = { -0.1f, 0.02f, 0.0f, 0.0f, 0.0f };
		PostProcess.VignetteIntensity = 0.3f;
		PostProcess.ChromaticAberration = 0.3f;
		PostProcess.MotionBlurAmount = 0.5f;
		PostProcess.BloomIntensity = 0.0f;
		PostProcess.LensFlareIntensity = 0.05f;
		Exposure = { true, 2.0f, 14.0f, 3.0f, 1.0f };
		Noise = { true, 0.0f, 0.02f, 0.01f };
		break;

	case ECameraSensorPreset::TeslaHW3_Narrow:
		Intrinsics = { 1280, 960, 36.0f, 35.0f, 8.0f };
		Distortion = { -0.05f, 0.01f, 0.0f, 0.0f, 0.0f };
		PostProcess.VignetteIntensity = 0.2f;
		PostProcess.ChromaticAberration = 0.2f;
		PostProcess.MotionBlurAmount = 0.5f;
		PostProcess.BloomIntensity = 0.0f;
		PostProcess.LensFlareIntensity = 0.05f;
		Exposure = { true, 2.0f, 14.0f, 3.0f, 1.0f };
		Noise = { true, 0.0f, 0.02f, 0.01f };
		break;

	case ECameraSensorPreset::TeslaHW4:
		Intrinsics = { 2560, 1920, 24.0f, 130.0f, 1.3f };
		Distortion = { -0.35f, 0.12f, 0.0f, 0.0f, 0.0f };
		PostProcess.VignetteIntensity = 0.35f;
		PostProcess.ChromaticAberration = 0.4f;
		PostProcess.MotionBlurAmount = 0.4f;
		PostProcess.BloomIntensity = 0.0f;
		PostProcess.LensFlareIntensity = 0.08f;
		Exposure = { true, 1.0f, 15.0f, 3.5f, 1.2f };
		Noise = { true, 0.0f, 0.015f, 0.008f };
		break;

	case ECameraSensorPreset::DroneFPV:
		Intrinsics = { 1920, 1080, 60.0f, 150.0f, 2.5f };
		Distortion = { -0.5f, 0.2f, -0.05f, 0.0f, 0.0f };
		PostProcess.VignetteIntensity = 0.5f;
		PostProcess.ChromaticAberration = 0.7f;
		PostProcess.MotionBlurAmount = 0.3f;
		PostProcess.BloomIntensity = 0.1f;
		PostProcess.LensFlareIntensity = 0.15f;
		Exposure = { true, 3.0f, 16.0f, 4.0f, 2.0f };
		Noise = { true, 0.0f, 0.025f, 0.015f };
		break;

	case ECameraSensorPreset::Waymo:
		Intrinsics = { 1920, 1280, 30.0f, 50.0f, 6.0f };
		Distortion = { -0.08f, 0.015f, 0.0f, 0.0f, 0.0f };
		PostProcess.VignetteIntensity = 0.2f;
		PostProcess.ChromaticAberration = 0.15f;
		PostProcess.MotionBlurAmount = 0.3f;
		PostProcess.BloomIntensity = 0.0f;
		PostProcess.LensFlareIntensity = 0.03f;
		Exposure = { true, 1.0f, 16.0f, 4.0f, 1.5f };
		Noise = { true, 0.0f, 0.01f, 0.005f };
		break;

	case ECameraSensorPreset::Custom:
	default:
		break;
	}
	
	RefreshSettings();
}

//카메라 프레임 저장 파이프라인
void UCameraSceneComponent::SaveCameraImage()
{
	//저장 폴더 생성
	const FString Dir = FPaths::ProjectSavedDir() / TEXT("SensorData") / DataSaveConfig.SensorLabel;
	IFileManager::Get().MakeDirectory(*Dir, true); //중간 폴더까지 전부 생성
	//프레임 카운터에 따른 사진 생성, 정렬 때문에 6자리 생성
	const FString FilePath = Dir / FString::Printf(TEXT("%06lld.jpg"), FrameCount);

	//RenderTarget은 GPU 텍스처라서, VRAM 안에 있어서, CPU가 직접 접근 X, RTResource가 직접 가져와서 읽는다.
	FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();
	if (!RTResource)
	{
		UE_LOG(LogCameraSensor, Warning, TEXT("RenderTarget resource unavailable for saving."));
		return;
	}

	const int32 Width = RenderTarget->SizeX;
	const int32 Height = RenderTarget->SizeY;

	TArray<FColor> Pixels;
	Pixels.SetNumUninitialized(Width * Height);
	if (!RTResource->ReadPixels(Pixels))
	{
		UE_LOG(LogCameraSensor, Warning, TEXT("ReadPixels failed for frame %lld"), FrameCount);
		return;
	}
	
	//Alpha 강제 설정
	for (FColor& Pixel : Pixels)
	{
		Pixel.A = 255;
	}
	//언리얼 이미지 압축 프로그램
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

	if (ImageWrapper.IsValid() == false)
	{
		UE_LOG(LogCameraSensor, Warning, TEXT("Failed to create JPEG ImageWrapper."));
		return;
	}

	ImageWrapper->SetRaw(Pixels.GetData(), Pixels.Num() * sizeof(FColor), Width, Height, ERGBFormat::BGRA, 8);
	const TArray64<uint8>& CompressedData = ImageWrapper->GetCompressed(DataSaveConfig.JPGQuality);
	/*Raw BGRA 픽셀 배열
		  ↓  SetRaw()     → 원본 데이터 등록 (포맷: BGRA, 비트깊이: 8)
		  ↓  GetCompressed() → JPEG 압축 실행 원본 데이터는 I/O가 너무 느리다.
	압축된 바이트 배열 (TArray64<uint8>) */
	FFileHelper::SaveArrayToFile(CompressedData, *FilePath);

	UE_LOG(LogCameraSensor, Verbose, TEXT("Saved %dx%d image → %s (%lld bytes)"),
		Width, Height, *FilePath, CompressedData.Num());
}