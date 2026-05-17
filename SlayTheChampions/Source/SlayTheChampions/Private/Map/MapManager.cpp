// Fill out your copyright notice in the Description page of Project Settings.

#include "Map/MapManager.h"

#include "Map/MapCreator.h"
#include "Map/RunSystem.h"

void UMapManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	MapCreator = NewObject<UMapCreator>(this, UMapCreator::StaticClass());
}

void UMapManager::StartRunSystem()
{
	URunSystem* RunSystem = GetGameInstance()->GetSubsystem<URunSystem>();
	if (RunSystem)
	{
		RunSystem->StartRunWithMap();
	}
}

void UMapManager::MapCreate()
{
	if (MapCreator)
	{
		MapCreator->CreateMap();
	}
}

void UMapManager::RestoreMapWorld()
{
	if (MapCreator)
	{
		//디버그용
		MapCreator->RestoreWorldMap();
	}
}

bool UMapManager::HasMapData() const
{
	return MapCreator ? MapCreator->HasMapData() : false;
}

void UMapManager::RefreshDebugMapState()
{
	if (MapCreator)
	{
		//디버그용
		MapCreator->RefreshDebugWorldMapState();
	}
}

UArea* UMapManager::GetAreaAt(int32 height, int32 width) const
{
	return MapCreator ? MapCreator->GetAreaAt(height, width) : nullptr;
}

int32 UMapManager::GetMapWidth() const
{
	return MapCreator ? MapCreator->GetMapWidth() : 0;
}

int32 UMapManager::GetMapHeight() const
{
	return MapCreator ? MapCreator->GetMapHeight() : 0;
}
