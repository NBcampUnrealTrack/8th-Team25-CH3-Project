// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "QuestCameraWidget.generated.h"

/**
 * 
 */
class UTextureRenderTarget2D;



UCLASS()
class ANTARCITCKIDS_API UQuestCameraWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "CameraSensor")
	void SetRenderTarget(UTextureRenderTarget2D* InRenderTarget);
	
	UFUNCTION(BlueprintCallable, Category= "CameraSensor")
	void ToggleCameraView();
	
	UFUNCTION(BlueprintCallable, Category= "CameraSensor")
	void TurnOnCameraView();
	
	UFUNCTION(BlueprintCallable, Category= "CameraSensor")
	void TurnOffCameraView();
	
	UFUNCTION(BlueprintPure, Category = "CameraSensor")
	bool IsCameraViewVisible() const;
	

	
private:

	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CameraImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> MissionCamBD;
	
	
};
