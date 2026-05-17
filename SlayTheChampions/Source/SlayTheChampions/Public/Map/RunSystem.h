// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Map/MapStruct.h"
#include "RunSystem.generated.h"

class UArea;
class UMapManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomSelectionChanged, const TArray<FAreaInfo>&, RoomInfos);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomEntered, const FAreaInfo&, RoomInfo);

UCLASS()
class SLAYTHECHAMPIONS_API URunSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
private:
	/*Run 진행상태*/
	UPROPERTY()
	ERunState CurrentRunState = ERunState::Ready;

	/*현재 Area의 정보*/
	UPROPERTY()
	FAreaInfo CurrentRoomInfo;

	/*이동가능한 Area을 가진지 여부*/
	UPROPERTY()
	bool bHasCurrentRoom = false;

	/*현재 Area*/
	UPROPERTY()
	UArea* CurrentArea = nullptr;

	/*Part 가져올 정보*/
	UPROPERTY()
	FRunPartySnapshot PartySnapshot;

	/*Deck 가져올 정보*/
	UPROPERTY()
	FRunDeckSnapshot DeckSnapshot;

	/*Relic 가져올 정보*/
	UPROPERTY()
	FRunRelicSnapshot RelicSnapshot;

	/*MapManager*/
	UPROPERTY()
	UMapManager* MapManager = nullptr;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/*현재 입장가능한 Area의 정보 델리게이트*/
	UPROPERTY(BlueprintAssignable, Category = "Run")
	FOnRoomSelectionChanged OnEnterableRoomsUpdated;

	/*Area 입장 델리게이트*/
	UPROPERTY(BlueprintAssignable, Category = "Run")
	FOnRoomEntered OnRoomEntered;

	/*각 Area 타입별 입장 델리게이트*/
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
	/*입장가능한 Area 정보 업데이트*/
	void UpdateEnterableState();
	/*Area 입장 처리*/
	void BroadcastRoomTypeEvent(const FAreaInfo& RoomInfo);
public:
	/*Run 시작*/
	UFUNCTION(BlueprintCallable)
	void StartRun();

	/*Run 시작 및 맵 생성*/
	UFUNCTION(BlueprintCallable)
	void StartRunWithMap();

	UFUNCTION(BlueprintCallable)
	void RefreshRunState();

	/*Area 입장*/
	UFUNCTION(BlueprintCallable)
	bool EnterRoom(UArea* Area);

	/*Area 입장 by GridIndex*/ 
	UFUNCTION(BlueprintCallable)
	bool EnterRoomByGridIndex(int32 FloorIndex, int32 RoomIndex);

	/*Area 입장 가능 여부 확인*/
	UFUNCTION(BlueprintCallable)
	bool CanEnterRoom(UArea* Area) const;

	/*Area 입장 가능 여부 확인 by GridIndex*/
	UFUNCTION(BlueprintCallable)
	bool CanEnterRoomByGridIndex(int32 FloorIndex, int32 RoomIndex) const;

	/*현재 입장가능한 Area 정보 반환*/
	UFUNCTION(BlueprintCallable)
	TArray<FAreaInfo> GetEnterableRooms() const;

	/*현재 Run 상태 반환*/
	UFUNCTION(BlueprintPure)
	ERunState GetRunState() const { return CurrentRunState; }

	/*현재 Area 정보 반환*/
	UFUNCTION(BlueprintPure)
	FAreaInfo GetCurrentRoomInfo() const { return CurrentRoomInfo; }

	/*Party 설정*/
	UFUNCTION(BlueprintCallable)
	void SetPartySnapshot(const FRunPartySnapshot& InSnapshot);

	/*Deck설정*/
	UFUNCTION(BlueprintCallable)
	void SetDeckSnapshot(const FRunDeckSnapshot& InSnapshot);

	/*Relic설정*/
	UFUNCTION(BlueprintCallable)
	void SetRelicSnapshot(const FRunRelicSnapshot& InSnapshot);

	UFUNCTION(BlueprintPure)
	FRunPartySnapshot GetPartySnapshot() const { return PartySnapshot; }

	UFUNCTION(BlueprintPure)
	FRunDeckSnapshot GetDeckSnapshot() const { return DeckSnapshot; }

	UFUNCTION(BlueprintPure)
	FRunRelicSnapshot GetRelicSnapshot() const { return RelicSnapshot; }
};
