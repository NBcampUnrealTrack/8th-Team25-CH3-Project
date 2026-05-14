#include "Sensor/LidarBevRenderer.h"
#include "Engine/Texture2D.h"

void ULidarBevRenderer::Initialize(const FBevRenderConfig& InConfig)
{
	Config = InConfig;
	CreateTexture();
	BuildColorLUT();
}

//텍스처 만들기
void ULidarBevRenderer::CreateTexture()
{
	//Config.ImageSize — 설정에서 가져온 이미지 해상도 (예: 512, 1024 등)
	const int32 Size = Config.ImageSize;
	//CreateTransient — 런타임에만 존재하는 임시 텍스처 생성 (에셋으로 저장 안 됨)
	DynamicTexture = UTexture2D::CreateTransient(Size, Size, PF_B8G8R8A8);
	//픽셀을 보간 없이 그대로 표시 → LiDAR BEV처럼 정확한 격자 데이터에 적합
	DynamicTexture->Filter = TF_Nearest;
	DynamicTexture->SRGB = true;
	//설정 변경 후 GPU에 텍스처 리소스를 실제로 등록/반영하는 호출
	DynamicTexture->UpdateResource();

	//PixelBuffer — CPU 메모리에 있는 픽셀 배열 (보통 TArray<FColor> 또는 TArray<uint32>)
	PixelBuffer.SetNumUninitialized(Size * Size);
	UpdateRegion = FUpdateTextureRegion2D(0, 0, 0, 0, Size, Size);
}

//텍스처를 만들기 위한 색깔 미리 준비
//많은 수를 칠해야 하기 때문에 미리 저장
void ULidarBevRenderer::BuildColorLUT()
{
	const FLinearColor DarkGreen(0.0f, 0.3f, 0.0f, 1.0f);
	const FLinearColor Bright = Config.PointColor;
	for (int32 i = 0; i < 256; ++i)
	{
		ColorLUT[i] = FMath::Lerp(
			DarkGreen,
			Bright,
			static_cast<float>(i) / 255.f
		).ToFColor(true);
	}
}

//업데이트 시에 미리 준비
void ULidarBevRenderer::UpdateConfig(const FBevRenderConfig& InConfig)
{
	const bool bSizeChanged = (Config.ImageSize != InConfig.ImageSize);
	Config = InConfig;
	BuildColorLUT();

	if (bSizeChanged)
	{
		CreateTexture();
	}
}

//포인터 클라우드 렌더링
void ULidarBevRenderer::RenderPointCloud(const FLidarPointCloudData& PointCloud, const FTransform& SensorTransform)
{
	if (!DynamicTexture) return;

	const int32 ImgSize = Config.ImageSize;
	const int32 TotalPixels = ImgSize * ImgSize;
	//이미지 정중앙에 흰점(센서 중앙)을 찍기 위해서
	const float HalfSize = static_cast<float>(ImgSize) * 0.5f;
	const float Scale = HalfSize / Config.ViewRange;
	//라이더의 센서 점 하나를 몇 픽셀로 그려야 하는가
	const int32 PtSize = FMath::Max(Config.PointSize, 1);
	const int32 PtHalf = PtSize / 2;

	const FColor BgColor = Config.BackgroundColor.ToFColor(true);
	FColor* RESTRICT Pixels = PixelBuffer.GetData();
	for (int32 i = 0; i < TotalPixels; ++i)
	{
		Pixels[i] = BgColor;
	}
	
	//월드 기준 좌표를 센서 기준 로컬 좌표로 바꾸는 중
	const FTransform InvSensor = SensorTransform.Inverse();
	const int32 PointCount = PointCloud.PointCount;
	const FVector* RESTRICT Points = PointCloud.Points.GetData();
	const float* RESTRICT Intensities = PointCloud.Intensities.GetData();
	const int32 IntensityCount = PointCloud.Intensities.Num();

	for (int32 i = 0; i < PointCount; ++i)
	{
		const FVector LocalPt = InvSensor.TransformPosition(Points[i]);
		//X,Y를 픽셀 단위에 맞도록 고치는 과정
		const int32 CX = FMath::RoundToInt32(HalfSize + LocalPt.Y * Scale);
		const int32 CY = FMath::RoundToInt32(HalfSize - LocalPt.X * Scale);

		if (CX < PtHalf || CX >= ImgSize - PtHalf || CY < PtHalf || CY >= ImgSize - PtHalf)
		{
			continue;
		}
		//인덱스가 있다면 그대로 처리하고 없으면 0.5 처리
		//Intensity가 없는 포인트를 "중간 밝기" 로 표시 → 데이터가 없어도 포인트가 보이긴 함
		const float Intensity = (i < IntensityCount) ? Intensities[i] : 0.5f;
		const FColor Color = ColorLUT[
			static_cast<uint8>(FMath::Clamp(Intensity * 255.f, 0.f, 255.f))
		];

		
		if (PtSize == 1)
		{
			//딱 한 픽셀만 찍는다.
			Pixels[CY * ImgSize + CX] = Color;
		}
		else
		{
			for (int32 dy = -PtHalf; dy < PtSize - PtHalf; ++dy)
			{
				const int32 Row = (CY + dy) * ImgSize;
				for (int32 dx = -PtHalf; dx < PtSize - PtHalf; ++dx)
				{
					Pixels[Row + CX + dx] = Color;
				}
			}
		}
	}

	//센서 중앙에 흰 점을 찍는다.
	const int32 C = FMath::RoundToInt32(HalfSize);
	const FColor White(255, 255, 255, 255);
	for (int32 dy = -3; dy < 3; ++dy)
	{
		const int32 Row = (C + dy) * ImgSize;
		for (int32 dx = -3; dx < 3; ++dx)
		{
			Pixels[Row + C + dx] = White;
		}
	}
	
	//GPU에 업로드한다.
	DynamicTexture->UpdateTextureRegions(
		0, 1, &UpdateRegion,
		ImgSize * sizeof(FColor),
		sizeof(FColor),
		reinterpret_cast<uint8*>(Pixels)
	);
}
