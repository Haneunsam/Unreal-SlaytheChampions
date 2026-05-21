#pragma once

#include "CoreMinimal.h"
#include "Map/MapEnum.h"
#include "MapStruct.generated.h"

/* Area 정보 */
USTRUCT(BlueprintType)
struct FAreaInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaInfo")
	EAreaState AreaState = EAreaState::Start;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaInfo")
	EAreaType AreaType = EAreaType::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaInfo")
	FVector2D AreaPos = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaInfo")
	EAreaVisitState AreaVisit = EAreaVisitState::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaInfo")
	bool bCanEnter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaInfo")
	bool bIsCurrentArea = false;

	FAreaInfo()
	{
	}

	FAreaInfo(EAreaState _state, EAreaType _type, FVector2D _arrPos, EAreaVisitState visit, bool bInCanEnter = false, bool bInIsCurrentArea = false)
		:AreaState(_state), AreaType(_type), AreaPos(_arrPos), AreaVisit(visit), bCanEnter(bInCanEnter), bIsCurrentArea(bInIsCurrentArea)
	{
	}
};

/* Map Area 배치 확률 */
USTRUCT(BlueprintType)
struct FAreaSpawnProbability
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaSpawnProbability")
	float Normal = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaSpawnProbability")
	float Elite = 16.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaSpawnProbability")
	float Event = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaSpawnProbability")
	float Rest = 12.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaSpawnProbability")
	float Shop = 5.f;

	FAreaSpawnProbability()
	{
	}
};

/* Map Area 고정 배치 위치 */
USTRUCT(BlueprintType)
struct FAreaFixedPlacement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FAreaFixedPlacement")
	int32 ArtifactEvent = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FAreaFixedPlacement")
	int32 Reword = 9;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FAreaFixedPlacement")
	int32 Rest = 14;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FAreaFixedPlacement")
	int32 Boss = 15;


	FAreaFixedPlacement()
	{
	}
};

/* 저장된 Area 데이터 */
USTRUCT(BlueprintType)
struct FSaveAreaData
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHasArea = false;

	UPROPERTY()
	FAreaInfo AreaInfo;

	UPROPERTY()
	TArray<int32> NextAreaIndices;
};
/* 저장된 Map 데이터 */
USTRUCT(BlueprintType)
struct  FSaveMapInfo
{
	GENERATED_BODY()
	/* Run 진행상태 */
	UPROPERTY()
	ERunState CurrentRunState = ERunState::Ready;

	/* 저장된 맵 가로폭 */
	UPROPERTY()
	int32 SavedMapWidth = 0;

	/* 저장된 맵 세로폭 */
	UPROPERTY()
	int32 SavedMapHeight = 0;

	/* 저장된 맵 데이터 유무 여부 */
	UPROPERTY()
	bool bHasSavedMap = false;

	/* 저장된 맵 데이터 배열 */
	UPROPERTY()
	TArray<FSaveAreaData> SavedMap;

	/* 현재 Area의 정보 */
	UPROPERTY()
	FAreaInfo CurrentRoomInfo;

	/* 현재 Area의 층 인덱스 */
	UPROPERTY()
	int32 CurrentFloorIndex = INDEX_NONE;

	/* 현재 Area의 방 인덱스 */
	UPROPERTY()
	int32 CurrentRoomIndex = INDEX_NONE;

	/* 이동했던 Area의 데이터 유무 여부 */
	UPROPERTY()
	bool bHasCurrentRoom = false;
};

/* 파티 시스템 스냅샷용 저장 데이터 */
USTRUCT(BlueprintType)
struct FRunPartySnapshot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run")
	TArray<FName> PartyMemberIds;
};

/* 카드 덱 시스템 스냅샷용 저장 데이터 */
USTRUCT(BlueprintType)
struct FRunDeckSnapshot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run")
	TArray<FName> DeckOwnerIds;
};

/* 유물 시스템 스냅샷용 저장 데이터 */
USTRUCT(BlueprintType)
struct FRunRelicSnapshot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run")
	TArray<FName> RelicIds;
};
