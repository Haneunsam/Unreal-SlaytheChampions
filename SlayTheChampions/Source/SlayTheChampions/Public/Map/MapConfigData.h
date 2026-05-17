// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Map/MapStruct.h"
#include "MapConfigData.generated.h"

UCLASS(BlueprintType)
class SLAYTHECHAMPIONS_API UMapConfigData : public UDataAsset
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Config")
    class UAreaLevelData* AreaLevelData = nullptr;

    UPROPERTY(EditAnywhere, Category = "Config")
    int32 MapWidth = 7;

    UPROPERTY(EditAnywhere, Category = "Config")
    int32 MapHeight = 15;

    UPROPERTY(EditAnywhere, Category = "Config", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float AreaSpawnProbability = 45.f;

    UPROPERTY(EditAnywhere, Category = "Config")
    FAreaSpawnProbability AreaTypeSpawnProbability;

    UPROPERTY(EditAnywhere, Category = "Config")
    FAreaFixedPlacement FixedPlacement;

    UPROPERTY(EditAnywhere, Category = "Config" )
    float TileDistance = 500.0f;

    UPROPERTY(EditAnywhere, Category = "Config")
    TSubclassOf<AActor> NormalMap;

    UPROPERTY(EditAnywhere, Category = "Config")
    TSubclassOf<AActor> EliteMap;

    UPROPERTY(EditAnywhere, Category = "Config")
    TSubclassOf<AActor> BossMap;

    UPROPERTY(EditAnywhere, Category = "Config")
    TSubclassOf<AActor> EventMap;

    UPROPERTY(EditAnywhere, Category = "Config")
    TSubclassOf<AActor> RestMap;

    UPROPERTY(EditAnywhere, Category = "Config")
    TSubclassOf<AActor> ShopMap;

    UPROPERTY(EditAnywhere, Category = "Config")
    TSubclassOf<AActor> RewordMap;

    UPROPERTY(EditAnywhere, Category = "Config")
    TSubclassOf<AActor> ArtifactEventMap;
};
