#pragma once

#include "CoreMinimal.h"
#include "Map/MapEnum.h"
#include "MapStruct.generated.h"

/*Area 정보*/
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

	FAreaInfo()
	{
	}

	FAreaInfo(EAreaState _state, EAreaType _type, FVector2D _arrPos, EAreaVisitState visit)
		:AreaState(_state), AreaType(_type), AreaPos(_arrPos), AreaVisit(visit)
	{
	}
};

/*Map Area배치 확률*/
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

/*Map Area 고정 배치 층수*/
USTRUCT(BlueprintType)
struct FAreaFixedPlacement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FAreaFixedPlacement")
	int32 ArtifactEvent = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FAreaFixedPlacement")
	int32 Reword = 9;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FAreaFixedPlacement")
	int32 Rest = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FAreaFixedPlacement")
	int32 Boss = 16;


	FAreaFixedPlacement()
	{
	}
};
