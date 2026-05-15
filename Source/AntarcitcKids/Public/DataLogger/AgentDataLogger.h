// AgentDataLogger.h
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#pragma once

#include "CoreMinimal.h" //�𸮾� �⺻ �ڷ���
#include "Components/ActorComponent.h" // UActorComponent
#include "AgentDataLogger.generated.h" // �𸮾� ���÷��� �ý���

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))

class ANTARCITCKIDS_API UAgentDataLogger : public UActorComponent
{
	GENERATED_BODY()

public:
	UAgentDataLogger(); //������
	void SetSteeringInputLog(double FrontLeftAngle, double FrontRightAngle); //�ܺο����� ���� ���� �� �ֵ��� �ϴ� �Լ�, �ڵ� ȸ����



protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override; 



private:
	
	//============����Ŭ ��/���� üũ��===================
	bool IsValidVehicleOwner() const;
	
	//====================================���Ⱚ ����=========================================
	double CurrentSteeringRightInput = 0.0; //������ ȸ������ ���� �Լ�
	double CurrentSteeringLeftInput = 0.0; //������ ȸ�� �� ���� �Լ�
	
	double GetSteeringRightValue() const; //������ �� ����
	double GetSteeringLeftValue() const; //���� �� ����
	//=============================================================
	
	
	// =============================�� �̵��Ÿ� ��� ����=============================
	
	FVector StartWorldLocation = FVector::ZeroVector; //ù �����ġ�� ����
	bool bHasStartedLocation = false; //�̹� ������ �ߴ°� ����� ����
	
	FVector PreviousWorldLocation = FVector::ZeroVector; 
	bool bHasPreviousWorldLocation = false;
	
	double GetDistanceFromStartM() const; //ù �������� �ٲ��� �����Ŵϱ�
	double TotalDistanceM = 0.0; //�޸��� ���� �ǽð����� ������
	double GetTotalDistanceM() const; //�� ����Ʈ��? ��� ���²� �޸��� ���� ���� ����ϴ� �Լ��� �Ұ���
	// =================================================================================================
	
	
	//=====================================����/���� ����====================================================
	
	double PreviousSpeedMps = 0.0; //���� �ӵ� ����
	bool bHasPreviousSpeed = false; 
	
	double CurrentAccelerationMps2 = 0.0; //���� ���ӵ�
	double GetAccelerationsMps2() const; //����
	
	double CurrentDecelerationMps2 = 0.0;//���� ���ӵ�
	double GetDecelerationMps2() const; //����
	//===================================================================================================
	
	
	static int32 GetUtmZone(double Longitude); //UTM�� ������ ���� �������� ������ ��ǥ��, �浵 �������� UTM Zone ��ȣ�� ���ϴ� �Լ�
	static void LatLonToUtm(double Lat, double Lon, int32 Zone, double& OutEasting, double& OutNorthing); //����, �浵�� UTM ��ǥ�� ��ȯ�ϴ� �Լ�
	void WorldToUtm(const FVector& WorldLocation, double& OutEasting, double& OutNorthing) const;
	void CreateCsvFile(); //Csv���� ���� �� ����
	void AppendRow();

	UFUNCTION(BlueprintCallable, Category = "Data Logger")
	void StartRecording(); //��ȭ ����

	UFUNCTION(BlueprintCallable, Category = "Data Logger")
	void StopRecording(); //��ȭ ��

	UFUNCTION(BlueprintPure, Category = "Data Logger")
	bool IsRecording() const { return bIsRecording; } //���ڵ���, ��ȭ������ Ȯ��



private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Logger",
		meta = (AllowPrivateAccess = "true"))
	bool bEnableLogging = true; //�α� ����� ����Ұ��ΰ�?, ���� �԰���

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Logger",
		meta = (ClampMin = "0.1", ClampMax = "100.0", Units = "Hz", AllowPrivateAccess = "true"))
	float SaveFrequencyHz = 10.0f; //�ʴ� �� ���� �����Ұ��ΰ�?, ���� �԰���

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Logger|UTM Reference",
		meta = (ClampMin = "-90.0", ClampMax = "90.0", Units = "deg", AllowPrivateAccess = "true"))
	double OriginLatitude = 36.4800; //�𸮾� ���� ����, ������ ���� �������� ��� �ش��ϴ°�? ������ ���� 37��, ���� �԰���

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Logger|UTM Reference",
		meta = (ClampMin = "-180.0", ClampMax = "180.0", Units = "deg", AllowPrivateAccess = "true"))
	double OriginLongitude = 127.0000; //���� �浵, �ѱ��� ���� �浵�� ���� 127��, ���� �԰���

private:
	double OriginUtmEasting = 0.0; //����
	double OriginUtmNorthing = 0.0; //�浵
	int32 OriginUtmZone = 0; // �𸮾� ���� �󿡼� UTM(���� 1~60�� ���ڰ� ����)


	//UTM ����� �����ϴ� ���� 

	FString CsvFilePath;
	bool bIsRecording = false;
	float TimeSinceLastSave = 0.0f;
	float ElapsedRecordingTime = 0.0f;

};
