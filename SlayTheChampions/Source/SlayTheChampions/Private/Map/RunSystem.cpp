// Fill out your copyright notice in the Description page of Project Settings.

#include "Map/RunSystem.h"
#include "Map/Area.h"
#include "Map/MapManager.h"

void URunSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	MapManager = GetGameInstance()->GetSubsystem<UMapManager>();
	CurrentRunState = ERunState::Ready;
	CurrentRoomInfo = FAreaInfo();
	bHasCurrentRoom = false;
	CurrentArea = nullptr;
	UpdateEnterableState();
}

void URunSystem::StartRun()
{
	CurrentRunState = ERunState::RunInit;
	CurrentRoomInfo = FAreaInfo();
	bHasCurrentRoom = false;
	CurrentArea = nullptr;
	UpdateEnterableState();
	OnEnterableRoomsUpdated.Broadcast(GetEnterableRooms());
}

void URunSystem::StartRunWithMap()
{
	if (MapManager)
	{
		MapManager->MapCreate();
	}

	StartRun();
}

void URunSystem::RefreshRunState()
{
	UpdateEnterableState();
	OnEnterableRoomsUpdated.Broadcast(GetEnterableRooms());
}

bool URunSystem::EnterRoom(UArea* Area)
{
	if (!CanEnterRoom(Area))
	{
		return false;
	}

	if (CurrentArea)
	{
		CurrentArea->SetCurrentArea(false);
	}

	CurrentArea = Area;
	CurrentArea->SetVisitState(EAreaVisitState::Visited);
	CurrentArea->SetState(EAreaState::Running);
	CurrentArea->SetCurrentArea(true);
	CurrentRoomInfo = CurrentArea->GetAreaInfo();
	bHasCurrentRoom = true;
	CurrentRunState = ERunState::RoomEntered;
	UpdateEnterableState();
	OnRoomEntered.Broadcast(CurrentRoomInfo);
	BroadcastRoomTypeEvent(CurrentRoomInfo);
	OnEnterableRoomsUpdated.Broadcast(GetEnterableRooms());
	return true;
}

bool URunSystem::EnterRoomByGridIndex(int32 FloorIndex, int32 RoomIndex)
{
	if (!MapManager)
	{
		return false;
	}

	return EnterRoom(MapManager->GetAreaAt(FloorIndex, RoomIndex));
}

bool URunSystem::CanEnterRoom(UArea* Area) const
{
	if (!Area)
	{
		return false;
	}

	const FAreaInfo& AreaInfo = Area->GetAreaInfo();
	if (AreaInfo.AreaVisit == EAreaVisitState::Visited || AreaInfo.AreaVisit == EAreaVisitState::Cleared)
	{
		return false;
	}

	if (!bHasCurrentRoom)
	{
		return static_cast<int32>(AreaInfo.AreaPos.X) == 0;
	}

	if (!CurrentArea)
	{
		return false;
	}

	for (UArea* NextArea : CurrentArea->GetNextAreas())
	{
		if (NextArea == Area)
		{
			return true;
		}
	}

	return false;
}

bool URunSystem::CanEnterRoomByGridIndex(int32 FloorIndex, int32 RoomIndex) const
{
	if (!MapManager)
	{
		return false;
	}

	return CanEnterRoom(MapManager->GetAreaAt(FloorIndex, RoomIndex));
}

TArray<FAreaInfo> URunSystem::GetEnterableRooms() const
{
	TArray<FAreaInfo> EnterableRooms;
	if (!MapManager)
	{
		return EnterableRooms;
	}

	for (int32 FloorIndex = 0; FloorIndex < MapManager->GetMapHeight(); FloorIndex++)
	{
		for (int32 RoomIndex = 0; RoomIndex < MapManager->GetMapWidth(); RoomIndex++)
		{
			UArea* Area = MapManager->GetAreaAt(FloorIndex, RoomIndex);
			if (Area && Area->GetAreaInfo().bCanEnter)
			{
				EnterableRooms.Add(Area->GetAreaInfo());
			}
		}
	}

	return EnterableRooms;
}

void URunSystem::SetPartySnapshot(const FRunPartySnapshot& InSnapshot)
{
	PartySnapshot = InSnapshot;
}

void URunSystem::SetDeckSnapshot(const FRunDeckSnapshot& InSnapshot)
{
	DeckSnapshot = InSnapshot;
}

void URunSystem::SetRelicSnapshot(const FRunRelicSnapshot& InSnapshot)
{
	RelicSnapshot = InSnapshot;
}

void URunSystem::UpdateEnterableState()
{
	if (!MapManager)
	{
		return;
	}

	for (int32 FloorIndex = 0; FloorIndex < MapManager->GetMapHeight(); FloorIndex++)
	{
		for (int32 RoomIndex = 0; RoomIndex < MapManager->GetMapWidth(); RoomIndex++)
		{
			UArea* Area = MapManager->GetAreaAt(FloorIndex, RoomIndex);
			if (!Area)
			{
				continue;
			}

			Area->SetCanEnter(CanEnterRoom(Area));
			Area->SetCurrentArea(Area == CurrentArea);
		}
	}

	CurrentRoomInfo = CurrentArea ? CurrentArea->GetAreaInfo() : FAreaInfo();
	MapManager->RefreshDebugMapState();
}

void URunSystem::BroadcastRoomTypeEvent(const FAreaInfo& RoomInfo)
{
	switch (RoomInfo.AreaType)
	{
	case EAreaType::Normal:
	case EAreaType::Elite:
	case EAreaType::Boss:
		OnBattleRoomEntered.Broadcast(RoomInfo);
		break;
	case EAreaType::Event:
		OnEventRoomEntered.Broadcast(RoomInfo);
		break;
	case EAreaType::Rest:
		OnRestRoomEntered.Broadcast(RoomInfo);
		break;
	case EAreaType::Shop:
		OnShopRoomEntered.Broadcast(RoomInfo);
		break;
	case EAreaType::Reword:
		OnRewardRoomEntered.Broadcast(RoomInfo);
		break;
	case EAreaType::ArtifactEvent:
		OnArtifactEventRoomEntered.Broadcast(RoomInfo);
		break;
	}
}
