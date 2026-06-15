// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Map/MapStruct.h"
#include "RunSystem.generated.h"

class UArea;
class UMapManager;
class URewardSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomSelectionChanged, const TArray<FAreaInfo>&, RoomInfos);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomEntered, const FAreaInfo&, RoomInfo);

UCLASS()
class SLAYTHECHAMPIONS_API URunSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	/* 저장되는 런/맵 진행 정보 */
	UPROPERTY()
	FSaveMapInfo MapInfo;

	/* 현재 플레이어가 머무는 방 */
	UPROPERTY()
	UArea* CurrentArea = nullptr;

	/* 런 저장용 파티 스냅샷 */
	UPROPERTY()
	FRunPartySnapshot PartySnapshot;

	/* 런 저장용 덱 스냅샷 */
	UPROPERTY()
	FRunDeckSnapshot DeckSnapshot;

	/* 런 저장용 유물 스냅샷 */
	UPROPERTY()
	FRunRelicSnapshot RelicSnapshot;

	/* 맵 데이터 생성과 조회를 담당하는 서브시스템 */
	UPROPERTY()
	UMapManager* MapManager = nullptr;

	/* 방 클리어 보상 UI를 여는 시스템 */
	UPROPERTY()
	URewardSystem* RewardSystem = nullptr;

	/* 방 클리어 후 복귀할 맵 서브레벨 이름 */
	UPROPERTY()
	FName MapLevelName = TEXT("RunMap");

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/* 현재 진입 가능한 방 목록이 바뀔 때 호출된다. 포탈 스포너가 이 이벤트를 받아 포탈을 다시 만든다. */
	UPROPERTY(BlueprintAssignable, Category = "Run")
	FOnRoomSelectionChanged OnEnterableRoomsUpdated;

	/* 방에 입장했을 때 호출되는 공통 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Run")
	FOnRoomEntered OnRoomEntered;

	/* 방 타입별 입장 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Run")
	FOnRoomEntered OnBattleRoomEntered;

	UPROPERTY(BlueprintAssignable, Category = "Run")
	FOnRoomEntered OnEventRoomEntered;

	UPROPERTY(BlueprintAssignable, Category = "Run")
	FOnRoomEntered OnRestRoomEntered;

	UPROPERTY(BlueprintAssignable, Category = "Run")
	FOnRoomEntered OnShopRoomEntered;

	UPROPERTY(BlueprintAssignable, Category = "Run")
	FOnRoomEntered OnRewardRoomEntered;

	UPROPERTY(BlueprintAssignable, Category = "Run")
	FOnRoomEntered OnArtifactEventRoomEntered;

private:
	/* 현재 방과 연결 정보를 기준으로 진입 가능한 방 상태를 갱신한다. */
	void UpdateEnterableState();

	/* 방 타입에 맞는 입장 이벤트를 호출한다. */
	void BroadcastRoomTypeEvent(const FAreaInfo& RoomInfo);

	/* 런 진행 데이터를 저장한다. */
	void SaveGameData();

public:
	/* 런을 처음 상태로 시작한다. */
	UFUNCTION(BlueprintCallable)
	void StartRun();

	/* 새 게임 시작 시 저장 복구용 런/맵 메모리 상태를 초기화한다. */
	UFUNCTION(BlueprintCallable)
	void ResetRunProgressForNewGame();

	/* 맵 데이터를 준비한 뒤 런을 시작한다. 저장된 맵이 있으면 이어서 복구한다. */
	UFUNCTION(BlueprintCallable)
	void StartRunWithMap();

	/* 저장된 현재 방 포인터와 진입 가능 방 상태를 다시 계산한다. */
	UFUNCTION(BlueprintCallable)
	void RefreshRunState();

	/* 지정한 방에 입장한다. */
	UFUNCTION(BlueprintCallable)
	bool EnterRoom(UArea* Area);

	/* 현재 방 클리어 처리 후 보상 UI를 연다. */
	UFUNCTION(BlueprintCallable)
	bool AreaCleared();

	/* 방 클리어 후 RunMap으로 복귀한다. */
	UFUNCTION(BlueprintCallable)
	void ReturnToMapAfterAreaClear();

	/* 맵 좌표로 방에 입장한다. */
	UFUNCTION(BlueprintCallable)
	bool EnterRoomByGridIndex(int32 FloorIndex, int32 RoomIndex);

	/* 지정한 방에 입장 가능한지 확인한다. */
	UFUNCTION(BlueprintCallable)
	bool CanEnterRoom(UArea* Area) const;

	/* 맵 좌표로 입장 가능 여부를 확인한다. */
	UFUNCTION(BlueprintCallable)
	bool CanEnterRoomByGridIndex(int32 FloorIndex, int32 RoomIndex) const;

	/* 현재 진입 가능한 방 목록을 반환한다. */
	UFUNCTION(BlueprintCallable)
	TArray<FAreaInfo> GetEnterableRooms() const;

	/* 현재 런 상태를 반환한다. */
	UFUNCTION(BlueprintPure)
	ERunState GetRunState() const { return MapInfo.CurrentRunState; }

	/* 현재 방 정보를 반환한다. */
	UFUNCTION(BlueprintPure)
	FAreaInfo GetCurrentRoomInfo() const { return MapInfo.CurrentRoomInfo; }

	/* 현재 층 인덱스를 반환한다. 내부 값은 0층부터 시작한다. */
	UFUNCTION(BlueprintPure)
	int32 GetCurrentFloorIndex() const { return MapInfo.CurrentFloorIndex; }

	/* UI 표시용 현재 층을 반환한다. 내부 0층을 화면에는 1층으로 보여준다. */
	UFUNCTION(BlueprintPure)
	int32 GetCurrentDisplayFloor() const { return MapInfo.CurrentFloorIndex == INDEX_NONE ? 0 : MapInfo.CurrentFloorIndex + 1; }

	/* 보상 시스템을 반환한다. */
	UFUNCTION(BlueprintPure)
	URewardSystem* GetRewardSystem() const { return RewardSystem; }

	/* 저장용 맵 정보를 반환한다. */
	UFUNCTION(BlueprintPure)
	FSaveMapInfo GetMapInfo() const;

	/* 저장된 맵 정보를 주입한다. */
	UFUNCTION(BlueprintCallable)
	void SetMapInfo(FSaveMapInfo _info);

	/* 복귀할 맵 레벨 이름을 설정한다. */
	UFUNCTION(BlueprintCallable)
	void SetMapLevelName(FName InMapLevelName) { MapLevelName = InMapLevelName; }

	/* 파티 스냅샷을 설정한다. */
	UFUNCTION(BlueprintCallable)
	void SetPartySnapshot(const FRunPartySnapshot& InSnapshot);

	/* 덱 스냅샷을 설정한다. */
	UFUNCTION(BlueprintCallable)
	void SetDeckSnapshot(const FRunDeckSnapshot& InSnapshot);

	/* 유물 스냅샷을 설정한다. */
	UFUNCTION(BlueprintCallable)
	void SetRelicSnapshot(const FRunRelicSnapshot& InSnapshot);

	UFUNCTION(BlueprintPure)
	FRunPartySnapshot GetPartySnapshot() const { return PartySnapshot; }

	UFUNCTION(BlueprintPure)
	FRunDeckSnapshot GetDeckSnapshot() const { return DeckSnapshot; }

	UFUNCTION(BlueprintPure)
	FRunRelicSnapshot GetRelicSnapshot() const { return RelicSnapshot; }
};
