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
	FName AreaName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaInfo")
	EAreaState AreaState = EAreaState::Start;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaInfo")
	EAreaType AreaType = EAreaType::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaInfo")
	FVector AreaWorldPos = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaInfo")
	FVector2D AreaArrPos = FVector2D::ZeroVector;

	EAreaVisitState AreaVisit = EAreaVisitState::None;
};

/*Map Area배치 확률*/
USTRUCT(BlueprintType)
struct FAreaSpawnProbability
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaSpawnProbability")
	float Normal = 45;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaSpawnProbability")
	float Elite = 16;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaSpawnProbability")
	float Event = 25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaSpawnProbability")
	float Rest = 12;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaSpawnProbability")
	float Shop = 5;
};
