// RoadTypes.h

#pragma once

#include "CoreMinimal.h"
#include "RoadTypes.generated.h"		// UENUM 매크로 사용을 위해


// 도로의 기본 분류 (1개만 선택) -> 도로의 기본 속도 제한을 결정.
// 언리얼 UENUM은 반드시 uint8로 지정해야 UHT가 처리할 수 있음
UENUM(BlueprintType)
enum class ERoadCategory : uint8
{
	Unspecified  UMETA(DisplayName = "Unspecified"),       // 미지정 (속도 제한 없음, 차량 MaxSpeed 사용)
	Highway      UMETA(DisplayName = "Highway"),           // 고속도로 (~110 km/h)
	Expressway   UMETA(DisplayName = "Expressway"),        // 자동차전용도로 (~90 km/h)
	Urban        UMETA(DisplayName = "Urban Road"),        // 일반 도시 도로 (~50 km/h)
	Residential  UMETA(DisplayName = "Residential"),       // 주택가/이면도로 (~30 km/h)
	Parking      UMETA(DisplayName = "Parking"),           // 주차장 내부 (~15 km/h)
	Service      UMETA(DisplayName = "Service Road"),      // 진입로/연결도로 (~30 km/h)
};


// 도로에 중첩 가능한 보호/특수 구역 플래그.
// 하나의 도로에 여러 플래그 동시 적용 가능. - 예: SchoolZone | ConstructionZone = 어린이보호구역인데 공사 중.
UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class ERoadZoneFlags : uint8
{
	None             = 0       UMETA(Hidden),
	SchoolZone       = 1 << 0  UMETA(DisplayName = "School Zone (어린이보호)"),
	SeniorZone       = 1 << 1  UMETA(DisplayName = "Senior Zone (노인보호)"),
	DisabledZone     = 1 << 2  UMETA(DisplayName = "Disabled Zone (장애인보호)"),
	ConstructionZone = 1 << 3  UMETA(DisplayName = "Construction"),
	AccidentProne    = 1 << 4  UMETA(DisplayName = "Accident-Prone Area"),
};
ENUM_CLASS_FLAGS(ERoadZoneFlags);	//  비트 연산자 오버로딩 매크로 (비트마스크 UENUM 만들 때 항상 같이 써야 하는 짝꿍)