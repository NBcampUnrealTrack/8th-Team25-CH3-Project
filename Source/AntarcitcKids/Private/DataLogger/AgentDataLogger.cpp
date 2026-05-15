//AgentDataLogger.cpp
// Copyright (c) 2026 AntarcticKids. All rights reserved.

#include "DataLogger/AgentDataLogger.h"
#include "HAL/PlatformFileManager.h" //파일을 다루는 헤더
#include "Misc/FileHelper.h" //저장과 읽기를 도와주는 헬퍼 헤더
#include "Misc/Paths.h" //프로젝트 경로 가져오기
#include "CityVehiclePawn.h"
#include "Vehicle/CyberTruckWheelFront.h"

//결국 최종 목표는 언리얼과 현실세계를 잇기 위한 실제 위도, 경도를 값으로 추출하는 과정

UAgentDataLogger::UAgentDataLogger()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UAgentDataLogger::BeginPlay()
{
	Super::BeginPlay();

	OriginUtmZone = GetUtmZone(OriginLongitude); //기준 경도, 127.0000
	LatLonToUtm(OriginLatitude, OriginLongitude, OriginUtmZone, OriginUtmEasting, OriginUtmNorthing); //위도 경도를 이용해서 UTM좌표로 변환하는 함수
	// 경도, OriginLatitude = 127.0000, double
	// 위도, OriginLongitude = 36.4800
	// OriginUtmZone = UTMZone 계산값
	//여기까지가 인풋

	// OriginUtmEasting = 위도
	// OriginUtmNorthing = 경도
	//여기가 아웃풋

	// 참조전달이므로 함수 안에서 값이 채워짐

	if (bEnableLogging)
	{
		StartRecording();
	} //로그 기능이 켜져있습니까?
} 

//여기가 기준 위도/경도를 UTM좌표로 변환 및 CSV파일 준비하는 과정, 보통 엑셀로 염, 실제 위치가 어디인가?


void UAgentDataLogger::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	StopRecording();
	Super::EndPlay(EndPlayReason);
}
//게임을 멈추거나 액터를 제거하면 기록도 멈춤


void UAgentDataLogger::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsRecording)
	{
		return;
	} //로그가 기록중이 아닙니까? 그럼 끌게~

	ElapsedRecordingTime += DeltaTime; //녹화 시작후 흐른 시간 누적, CSV파일에 시간값으로 들어가며, Timestamp의 형태로 저장
	TimeSinceLastSave += DeltaTime; //저장할 시간이 되었습니까? 를 판단

	const float SaveInterval = 1.0f / FMath::Max(SaveFrequencyHz, 0.1f); // 초당 10프레임으로 저장했으므로, 1.0 / 10.0 = 0.1초, 즉, 0.1초마다 저장
	if (TimeSinceLastSave >= SaveInterval)
	{
		AppendRow();
		TimeSinceLastSave -= SaveInterval;
	} // 저장시간 기록
}



void UAgentDataLogger::StartRecording()
{
	if (bIsRecording) 
	{
		return;
	} //중복시작을 막는 장치

	CreateCsvFile();
	bIsRecording = true;
	TimeSinceLastSave = 0.0f;
	ElapsedRecordingTime = 0.0f;
}
//CSV파일을 제작하여 기록 시작 함수



void UAgentDataLogger::StopRecording()
{
	bIsRecording = false;
}
//기록을 멈추는 함수

//============공통 검증함수==================================
bool UAgentDataLogger::IsValidVehicleOwner() const
{
	const AActor* Owner = GetOwner();
	
	if (!Owner)
	{
		return false;
	}
	
	return Owner->IsA<AWheeledVehiclePawn>();
}
//=========================================================


//===========================================자동차 핸들 회전===============================================


void UAgentDataLogger::SetSteeringInputLog(double FrontLeftAngle, double FrontRightAngle)
{

	if (!IsValidVehicleOwner())
	{
		return;
	}
	
	CurrentSteeringLeftInput = FrontLeftAngle; //여기에 값이 저장
	CurrentSteeringRightInput = FrontRightAngle; //여기에 값이 저장
}
//Owner가 진짜 차량인지 아닌지 확인후, 앞바퀴의 왼쪽, 오른쪽 각도를 부르는 함수


double UAgentDataLogger::GetSteeringLeftValue() const
{
	return CurrentSteeringLeftInput;
}


double UAgentDataLogger::GetSteeringRightValue() const
{
	return CurrentSteeringRightInput;
}

//외부에서 이미 저장해둔 값을 읽어서 반환하는 함수
//조회하는 통로라서, 아직 아무런 값도 뜨지 않음
//===================================================================================================

//===========================================총 이동거리===================================================


double UAgentDataLogger::GetDistanceFromStartM() const
{
	if (!GetOwner())
	{
		return 0.0;
	}
	
	return FVector::Dist(
		StartWorldLocation,
		GetOwner()->GetActorLocation()
		)*0.01; //언리얼 내부에서 자동차가 이동한 거리의 값을 추출, 값을 보낸 후 m변환
}


double UAgentDataLogger::GetTotalDistanceM() const
{
	return TotalDistanceM;
} //총 이동거리 기억

//====================================================================================================

//=====================가속도 관련====================================================
double UAgentDataLogger::GetAccelerationsMps2() const
{
	return CurrentAccelerationMps2;
}

double UAgentDataLogger::GetDecelerationMps2() const
{
	return CurrentDecelerationMps2;
}

//================================================================================



void UAgentDataLogger::CreateCsvFile()
{
	const FString OutputDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Output")); //프로젝트 폴더 안에 Output폴더경로를 제작
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile(); //파일이나 폴더를 만들 때 사용함
	PlatformFile.CreateDirectoryTree(*OutputDir); // OutPut폴더를 제작

	const FDateTime Now = FDateTime::Now(); //현제시간 가져오기
	const FString FileName = FString::Printf(
		TEXT("AgentData-%04d_%02d-%02d-%02d-%02d-%02d.csv"),
		Now.GetYear(), Now.GetMonth(), Now.GetDay(),
		Now.GetHour(), Now.GetMinute(), Now.GetSecond()
	); // 현제 날짜, 시간을 통해 파일 이름을 제작함
	// 파일이름은 파일이름-2026_05-08-02-22-54.csv의 형태로 기록

	CsvFilePath = FPaths::Combine(OutputDir, FileName); //Csv파일이 Path에 저장, 경로와 파일 이름을 합쳐 최종 csv경로를 제작
	//여기까지가 csv경로를 제작하는 과정

	const FString Header =
		TEXT("Timestamp,World_X,World_Y,World_Z,UTM_Easting,UTM_Northing,UTM_Zone,Velocity_kmh,Yaw,pitch,Roll,F_LeftValue,F_RightValue,Total_M,Start_M,CurrentSpeed,Acceleration,Deceleration\n"
		); //CSV 파일 내부에 작성 될 순서와 작성돨 것, 차례대로 시간 경과, 언리얼 월드 X,Y,Z, 실제 UTM기반 좌표,, UTM 구역 번호를 나타냄, 속도, 회전각
	FFileHelper::SaveStringToFile(Header, *CsvFilePath,
		FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM
	); //"Timestamp,World_X,World_Y,World_Z,UTM_Easting,UTM_Northing,UTM_Zone,Velocity_kmh,Yaw\n" 파일의 첫줄에 이렇게 들어감

	UE_LOG(LogTemp, Log, TEXT("[AgentDataLogger] Recording to: %s  (%.1f Hz)"), *CsvFilePath, SaveFrequencyHz); //언리얼 아웃풋 로그에서 기록 시작
}
//CSV 파일을 제작하는 함수 



void UAgentDataLogger::AppendRow()
{
	const AActor* Owner = GetOwner(); //컴포넌트가 붙어있는 엑터 들고오기
	if (!Owner)
		return; //없으면 종료

	const FVector WorldLoc = Owner->GetActorLocation(); // 언리얼 상에선 cm단위이며 월드 위치를 들고옴
	
	//====================첫 출발위치 저장================================
	if (!bHasStartedLocation)
	{
		StartWorldLocation = WorldLoc;
		bHasStartedLocation = true;
	}
	const double DistanceFromStartM = FVector::Dist(StartWorldLocation, WorldLoc) * 0.01; 
	//=================================================================
	//첫 출발점 기준에서 차량이 얼마나 멀어졌는지 확인하는 값	
	
	//====================================총 이동관련=======================================
	if (bHasPreviousWorldLocation)
	{
		const double DeltaDistanceM =
			FVector::Dist(PreviousWorldLocation, WorldLoc)*0.01; //이전 프레임에서 얼마나 이동했는지, cm->m로 변환하는 값
		
		TotalDistanceM += DeltaDistanceM; //누적거리 계산
	}
	
	PreviousWorldLocation = WorldLoc; //다음 프레임의 이전 위치
	bHasPreviousWorldLocation = true; // 거리계산 가능, 최초위치 저장완료
	//====================================================================================
	
	
	const FRotator WorldRot = Owner->GetActorRotation(); // 회전값
	const FVector Velocity = Owner->GetVelocity(); // 엑터의 속도를 가져오며 언리얼 단위는 cm이므로 cm/s임

	const double CurrentSpeedMps = Velocity.Size() * 0.01;
	const double SpeedKmh = Velocity.Size() * 3.6;
	// 0.01은 cm/s -> m/s로, 3.6은 m/s -> km/h로 변환함
	
	//=====================현제 가속도 계산=========================================================

	if (bHasPreviousSpeed)
	{
		const double DeltaSpeedM = CurrentSpeedMps - PreviousSpeedMps;
		const double DeltaTimeSeconds = 1.0 / FMath::Max(SaveFrequencyHz, 0.1f);
		
		CurrentAccelerationMps2 = DeltaSpeedM / DeltaTimeSeconds;
	}
	
	PreviousSpeedMps = CurrentSpeedMps;
	bHasPreviousSpeed = true;
	
	//====감속도 계산========
	
	if (CurrentAccelerationMps2 < 0.0)
	{
		CurrentDecelerationMps2 = FMath::Abs(CurrentAccelerationMps2);
	}

	else
	{
		CurrentDecelerationMps2 = 0.0;
	}
	
	//===========================================================================================

	const double Yaw = WorldRot.Yaw; //차량의 방향을 저장
	const double Pitch = WorldRot.Pitch;
	const double Roll = WorldRot.Roll;
	
	const double FrontLeftSteering = GetSteeringLeftValue(); //조향값
	const double FrontRightSteering = GetSteeringRightValue(); //조향값
	
	
	
	double UtmEasting = 0.0;
	double UtmNorthing = 0.0;
	//UTM 좌표 결과를 받을 변수
	WorldToUtm(WorldLoc, UtmEasting, UtmNorthing);
	//언리얼 월드 좌표를 UTM 좌표로 변환함, UtmEasting, UtmNorthing로 값이 들어감

	const FString Row = FString::Printf(
	TEXT("%.3f,%.2f,%.2f,%.2f,%.4f,%.4f,%d,%.2f,%.4f,%.4f,%.4f,%.4f,%.4f,%.3f,%.3f,%.4f,%.4f,%.4f\n"),
		ElapsedRecordingTime,
		WorldLoc.X, WorldLoc.Y, WorldLoc.Z,
		UtmEasting, UtmNorthing, OriginUtmZone,
		SpeedKmh,
		Yaw,
		Pitch,
	    Roll,
	    FrontLeftSteering,
	    FrontRightSteering,
	    GetTotalDistanceM(),
	    DistanceFromStartM,
	    CurrentSpeedMps,
	    GetAccelerationsMps2(),
	    GetDecelerationMps2()
	);
	//CSV에 들어갈 한 줄 문자열을 제작

	FFileHelper::SaveStringToFile(Row, *CsvFilePath,
		FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM,
		&IFileManager::Get(),
		EFileWrite::FILEWRITE_Append
	);
//파일이 덮어쓰이지 않고, 하나 하나 생성
}
// 실제 데이터를 한줄씩 저장


void UAgentDataLogger::WorldToUtm(const FVector& WorldLocation,
	double& OutEasting, double& OutNorthing) const //언리얼 월드 좌표 -> 실제 UTM좌표 변환
{
	const double OffsetEastM = WorldLocation.X * 0.01; //cm이므로 m로 바꾸기 위한 곱, 동쪽
	const double OffsetNorthM = -WorldLocation.Y * 0.01; //cm이므로 m로 바꾸기 위한 곱, 북쪽
	//-y인 이유 : 프로젝트 좌표계에서 y의 방향이 남쪽 방향이기 때문
	//맵 제작 방향, 차량 진행 방향, GIS 데이터 축 방향에 따라서 WorldLocation.X,Y * 0.01;는 달라짐, 맵 기준으로 바뀔 예정

	OutEasting = OriginUtmEasting + OffsetEastM; 
	OutNorthing = OriginUtmNorthing + OffsetNorthM; 
	//기존 UTM좌표에 월드 좌표를 더함
	//촤종 UTM = 기존UTM + 언리얼 월드 이동량
}
//언리얼 월드 좌표 -> 실제 UTM좌표 변환



int32 UAgentDataLogger::GetUtmZone(double Longitude)
{
	return FMath::FloorToInt((Longitude + 180.0) / 6.0) + 1;
}
//UTM Zone계산
//왜 6인가? UTM은 경도를 기준으로 6도씩 나눔


void UAgentDataLogger::LatLonToUtm(double Lat, double Lon, int32 Zone, double& OutEasting, double& OutNorthing) //지구 좌표를 평면 좌표UTM으로 변환, 절때 언리얼 상이 아님 
{
	//실제 지구 위 좌표를 UTM이라는 2차원상의 좌표로 변환시키는 과정임
	//WGS84: 위도, 경도로 표현하는 gps좌표(구 기준)
	//UTM: 미터단위로 표현하는 평면 좌표 UTM, 60개의 구역으로 나눔
	//구글 지도랑 같이 보면 좋다


	constexpr double a = 6378137.0; //실제 지구 반지름, m단위
	constexpr double f = 1.0 / 298.257223563; //타원체의 값, 지구의 모양과 크기를 근사적으로 나타내는 수학적 도형, 약간 납작하데요
	constexpr double k0 = 0.9996;// UTM 축척 계수

	const double e2 = 2.0 * f - f * f; //타원체의 찌그러짐을 계산
	const double ep2 = e2 / (1.0 - e2); //UTM 공식에서 보정항으로 사용

	const double LatRad = FMath::DegreesToRadians(Lat); //라디안 값, 원의 반지름
	const double CentralMeridian = (Zone - 1) * 6.0 - 180.0 + 3.0; //중앙경선을 구하는 값
	const double DeltaLon = FMath::DegreesToRadians(Lon - CentralMeridian); //경도와 중앙경산의 차이를 구하고 라디안 값으로 변환


	const double SinLat = FMath::Sin(LatRad);
	const double CosLat = FMath::Cos(LatRad);
	const double TanLat = FMath::Tan(LatRad);
	//위도의 sin cos tan의 값을 계산

	const double N = a / FMath::Sqrt(1.0 - e2 * SinLat * SinLat); //지구 타원체 휘어진 정도의 값
	const double T = TanLat * TanLat;// 탄젠트 제곱
	const double C = ep2 * CosLat * CosLat;// 코사인 제곱
	const double A = CosLat * DeltaLon;// 중앙선으로부터 얼마나 떨어져 있는지
	//언리얼 값을 지구의 값으로 치환, 얼마나 일그러져있는가?


	//자오선호 계산, 지구 표면에서 두 지점 사이의 북남 방향 곡선 거리
	const double e4 = e2 * e2;
	const double e6 = e4 * e2;
	//이심률의 4제곱, 6제곱을 계산


	const double M = a * (
		(1.0 - e2 / 4.0 - 3.0 * e4 / 64.0 - 5.0 * e6 / 256.0) * LatRad
		- (3.0 * e2 / 8.0 + 3.0 * e4 / 32.0 + 45.0 * e6 / 1024.0) * FMath::Sin(2.0 * LatRad)
		+ (15.0 * e4 / 256.0 + 45.0 * e6 / 1024.0) * FMath::Sin(4.0 * LatRad)
		- (35.0 * e6 / 3072.0) * FMath::Sin(6.0 * LatRad));



	const double A2 = A * A;
	const double A4 = A2 * A2;
	const double A6 = A4 * A2;


	OutEasting = k0 * N * (
		A
		+ (1.0 - T + C) * A2 * A / 6.0
		+ (5.0 - 18.0 * T + T * T + 72.0 * C - 58.0 * ep2) * A4 * A / 120.0
		) + 500000.0; 


	OutNorthing = k0 * (M + N * TanLat * (
		A2 / 2.0
		+ (5.0 - T + 9.0 * C + 4.0 * C * C) * A4 / 24.0
		+ (61.0 - 58.0 * T + T * T + 600.0 * C - 330.0 * ep2) * A6 / 720.0
		));



	if (Lat < 0.0)
		OutNorthing += 10000000.0;
}
//실제 지구 좌표 기준 위도/경도를 UTM좌표로 변환

//더 추가해볼법한것
//1. 가속도, 얼마나 급 가/감속했는가
//2. 차량의 앞뒤 기울기, 경사로 언덕 등
//3. 핸들을 얼마나 꺾었을까?
//4. Ai가 차량을 어떻게 제어했는가?
//5. 총 이동거리는 어떠한가?
