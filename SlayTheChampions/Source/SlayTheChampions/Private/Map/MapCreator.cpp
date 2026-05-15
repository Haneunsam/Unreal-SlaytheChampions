// Fill out your copyright notice in the Description page of Project Settings.

#include "Map/MapCreator.h"
#include "Map/Area.h"
#include "Map/MapStruct.h"
#include "Map/MapConfigData.h"

UMapCreator::UMapCreator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UMapCreator::~UMapCreator()
{
}

void UMapCreator::InitWorldMap()
{
	for (AActor* Actor : WorldMap)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	WorldMap.Empty();
}

bool UMapCreator::LoadDefaultConfig()
{
	if (!MapConfig)
	{
		MapConfig = LoadObject<UMapConfigData>(nullptr, TEXT("/Game/04_Data/MapCreatorData.MapCreatorData"));
	}

	if (!MapConfig)
	{
		UE_LOG(LogTemp, Error, TEXT("UMapCreator: MapCreatorData load failed."));
		return false;
	}

	CurrentWidth = MapConfig->MapWidth;
	CurrentHeight = MapConfig->MapHeight;
	return true;
}

void UMapCreator::CreateMap()
{
	if (!LoadDefaultConfig()) return;

	//맵 생성 작업전 정보 담을 배열들 초기화
	InitMap();
	InitGridMap();
	InitWorldMap();

	// 그리드 맵 생성
	GridMapCreate(CurrentWidth, CurrentHeight, MapConfig->AreaSpawnProbability);

	//Map에 데이터 입력
	SetMapData(CurrentWidth, CurrentHeight);

	//Area 연결
	ConnectAreas(CurrentWidth, CurrentHeight);

	//디버그용
	for (int i = 0; i < Map.Num(); i++)
	{
		if (Map[i] == nullptr) continue;

		Map[i]->DebugShowInfo();
	}

	//실제 맵 생성
	WorldMapCreate(CurrentWidth, CurrentHeight);
}

#pragma region GridMapGeneration
void UMapCreator::GridMapCreate(int32 MapWidth, int32 MapHeight, float AreaSpawnProbability)
{

	//1층 방 세팅
	GridMap[MapWidth / 2] = true;

	//중간방들 세팅
	for (int height = 1; height < MapHeight - 1; height++)
	{
		int32 FloorStart = height * MapWidth;
		int32 MustHavePos = FloorStart + FMath::RandRange(0, MapWidth - 1);
		GridMap[MustHavePos] = true;

		for (int32 w = 0; w < MapWidth; w++)
		{
			if (FMath::RandRange(0, 100) < AreaSpawnProbability)
			{
				GridMap[FloorStart + w] = true;
			}
		}
	}

	//마지막 층 방 세팅

	int32 LastStart = (MapHeight - 1) * MapWidth;
	int32 LastCenter = LastStart + (MapWidth / 2);
	GridMap[LastCenter] = true;

	//디버그용
	/*FString DebugMapString = TEXT("\n--- Grid Map Debug ---\n");
	for (int32 y = 0; y < MapHeight; y++)
	{
		FString Line = TEXT("");
		for (int32 x = 0; x < MapWidth; x++)
		{
			int32 Index = (y * MapWidth) + x;
			Line += GridMap[Index] ? TEXT(" 1 ") : TEXT(" 0 ");
		}
		DebugMapString += Line + TEXT("\n");
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *DebugMapString);*/
}
#pragma endregion

#pragma region MapDataSetting
void UMapCreator::SetMapData(int32 MapWidth, int32 MapHeight)
{
	for (int height = 0; height < MapHeight; height++)
	{
		for (int width = 0; width < MapWidth; width++)
		{
			int32 pos = (height * MapWidth) + width;

			if (GridMap[pos])
			{
				Map[pos] = AreaCreate(height, width);
			}
		}
	}
}

UArea* UMapCreator::AreaCreate(int32 height, int32 width)
{
	UArea* Area = NewObject<UArea>(this);

	EAreaType type = EAreaType::None;
	FAreaFixedPlacement FixedPlacement;
	int32 floor = height + 1;

	if ((floor == FixedPlacement.ArtifactEvent)) type = EAreaType::ArtifactEvent;
	else if ((floor == FixedPlacement.Reword)) type = EAreaType::Reword;
	else if ((floor == FixedPlacement.Rest)) type = EAreaType::Rest;
	else if ((floor == FixedPlacement.Boss)) type = EAreaType::Boss;

	if (type == EAreaType::None)type = GetRandAreaType();

	FAreaInfo AreaInfo(
		EAreaState::Ready,
		type,
		FVector2D(height, width),
		EAreaVisitState::None);

	Area->InitArea(AreaInfo);

	return Area;
}

EAreaType UMapCreator::GetRandAreaType()
{
	FAreaSpawnProbability Prob;

	float RandVal = FMath::FRandRange(1.f, 100.f);

	if ((RandVal -= Prob.Normal) <= 0.f) return EAreaType::Normal;
	if ((RandVal -= Prob.Elite) <= 0.f) return EAreaType::Elite;
	if ((RandVal -= Prob.Event) <= 0.f) return EAreaType::Event;
	if ((RandVal -= Prob.Rest) <= 0.f) return EAreaType::Rest;

	return EAreaType::Shop;
}
#pragma endregion

#pragma region MapConnect
void UMapCreator::ConnectAreas(int32 MapWidth, int32 MapHeight)
{
	for (int height = 0; height < MapHeight - 1; height++)
	{
		for (int width = 0; width < MapWidth; width++)
		{
			int32 CurrnetPos = (height * MapWidth) + width;
			int32 NextHeight = height + 1;
			if (Map[CurrnetPos] == nullptr) continue;

			//1층일경우 2층 모든방과 연결
			if (height == 0)
			{
				for (int32 nextW = 0; nextW < MapWidth; nextW++)
				{
					int32 NextIdx = (NextHeight * MapWidth) + nextW;
					if (Map[NextIdx])
					{
						Map[CurrnetPos]->SetNextAreas(Map[NextIdx]);
					}
				}
				continue;
			}
			//마지막 직전 층일경우 전부 보스방과 연결
			if (NextHeight == MapHeight - 1)
			{
				int32 BossIdx = (NextHeight * MapWidth) + (MapWidth / 2);
				if (Map[BossIdx])
				{
					Map[CurrnetPos]->SetNextAreas(Map[BossIdx]);
				}
				continue;
			}

			//나머지방
			bool isConnect = false;
			for (int32 dw = -1; dw <= 1; dw++)
			{
				int32 NextWidth = width + dw;
				if (ConnectIfValid(CurrnetPos, NextHeight, NextWidth, MapWidth))
				{
					isConnect = true;
				}
			}

			if (!isConnect)
			{
				for (int32 dist = 2; dist < MapWidth; dist++)
				{
					if (ConnectIfValid(CurrnetPos, NextHeight, width - dist, MapWidth)) break;
					if (ConnectIfValid(CurrnetPos, NextHeight, width + dist, MapWidth)) break;
				}
			}
		}
	}
}

bool UMapCreator::ConnectIfValid(int32 CurrnetPos, int32 NextHeight, int32 NextWidth, int32 MapWidth)
{
	if (NextWidth >= 0 && NextWidth < MapWidth)
	{
		int32 NextIdx = (NextHeight * MapWidth) + NextWidth;
		if (Map[NextIdx])
		{
			Map[CurrnetPos]->SetNextAreas(Map[NextIdx]);
			return true;
		}
	}
	return false;
}
#pragma endregion

#pragma region WorldMapGeneration
void UMapCreator::WorldMapCreate(int32 MapWidth, int32 MapHeight)
{
	UWorld* World = GetWorld();
	if (!World || !MapConfig) return;



	for (int32 height = 0; height < MapHeight; height++)
	{
		for (int32 width = 0; width < MapWidth; width++)
		{
			int32 CurrentPos = (height * MapWidth) + width;

			if (!Map.IsValidIndex(CurrentPos) || Map[CurrentPos] == nullptr) continue;

			FVector SpawnLocation = FVector(width * MapConfig->TileDistance, height * MapConfig->TileDistance, 0.0f);
			FRotator SpawnRotation = FRotator::ZeroRotator;

			TSubclassOf<AActor> SelectedClass = GetAreaClass(Map[CurrentPos]->GetType());

			if (SelectedClass)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				const FString AreaTypeName = StaticEnum<EAreaType>()->GetNameStringByValue(static_cast<int64>(Map[CurrentPos]->GetType()));
				const FString AreaName = FString::Printf(TEXT("%d층 %d번 %s"), height, width, *AreaTypeName);
				AActor* SpawnedActor = World->SpawnActor<AActor>(SelectedClass, SpawnLocation, SpawnRotation, SpawnParams);
				if (SpawnedActor)
				{
#if WITH_EDITOR
					SpawnedActor->SetActorLabel(AreaName);
#endif
					WorldMap.Add(SpawnedActor);
				}
			}
		}
	}
}
TSubclassOf<AActor> UMapCreator::GetAreaClass(EAreaType type)
{
	switch (type)
	{
	case EAreaType::Normal: return MapConfig->NormalMap;  break;
	case EAreaType::Elite:return MapConfig->EliteMap; break;
	case EAreaType::Boss:return MapConfig->BossMap; break;
	case EAreaType::Event:return MapConfig->EventMap; break;
	case EAreaType::Rest:return MapConfig->RestMap; break;
	case EAreaType::Shop:return MapConfig->ShopMap; break;
	case EAreaType::Reword:return MapConfig->RewordMap; break;
	case EAreaType::ArtifactEvent:return MapConfig->ArtifactEventMap; break;
	}
	return MapConfig->NormalMap;
}
#pragma endregion