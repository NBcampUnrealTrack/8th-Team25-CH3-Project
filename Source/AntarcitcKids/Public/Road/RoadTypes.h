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