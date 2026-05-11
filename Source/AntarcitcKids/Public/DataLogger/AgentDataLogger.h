//AgentDataLogger.h

#pragma once

#include "CoreMinimal.h" //언리얼 기본 자료형
#include "Components/ActorComponent.h" // UActorComponent
#include "AgentDataLogger.generated.h" // 언리얼 리플렉션 시스템

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))

class ANTARCITCKIDS_API UAgentDataLogger : public UActorComponent
{
	GENERATED_BODY()

public:
	UAgentDataLogger(); //생성자 

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override; //핵심

private:
	static int32 GetUtmZone(double Longitude); //UTM은 지구를 세로 구역으로 나누는 좌표계, 경도 기준으로 UTM Zone 번호를 구하는 함수
	static void LatLonToUtm(double Lat, double Lon, int32 Zone, double& OutEasting, double& OutNorthing); //위도, 경도를 UTM 좌표로 변환하는 함수
	void WorldToUtm(const FVector& WorldLocation, double& OutEasting, double& OutNorthing) const;
	void CreateCsvFile(); //Csv파일 생성 및 제작
	void AppendRow();

	UFUNCTION(BlueprintCallable, Category = "Data Logger")
	void StartRecording(); //녹화 시작

	UFUNCTION(BlueprintCallable, Category = "Data Logger")
	void StopRecording(); //녹화 끝

	UFUNCTION(BlueprintPure, Category = "Data Logger")
	bool IsRecording() const { return bIsRecording; } //레코딩중, 녹화중인지 확인

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Logger",
		meta = (AllowPrivateAccess = "true"))
	bool bEnableLogging = true; //로그 기능을 사용할것인가?, 조절 쌉가능

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Logger",
		meta = (ClampMin = "0.1", ClampMax = "100.0", Units = "Hz", AllowPrivateAccess = "true"))
	float SaveFrequencyHz = 10.0f; //초당 몇 번을 저장할것인가?, 조절 쌉가능

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Logger|UTM Reference",
		meta = (ClampMin = "-90.0", ClampMax = "90.0", Units = "deg", AllowPrivateAccess = "true"))
	double OriginLatitude = 36.4800; //언리얼 기준 위도, 원점이 실제 지구상의 어디에 해당하는가? 기준은 북위 37도, 조절 쌉가능

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Logger|UTM Reference",
		meta = (ClampMin = "-180.0", ClampMax = "180.0", Units = "deg", AllowPrivateAccess = "true"))
	double OriginLongitude = 127.0000; //기준 경도, 한국의 기준 경도는 동경 127도, 조절 쌉가능

private:
	double OriginUtmEasting = 0.0; //위도
	double OriginUtmNorthing = 0.0; //경도
	int32 OriginUtmZone = 0; // 언리얼 엔진 상에서 UTM(보통 1~60의 숫자가 쓰임)


	//UTM 계산후 저장하는 변수 

	FString CsvFilePath;
	bool bIsRecording = false;
	float TimeSinceLastSave = 0.0f;
	float ElapsedRecordingTime = 0.0f;

};
