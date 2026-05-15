// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MapConfigData.generated.h"

UCLASS(BlueprintType)
class SLAYTHECHAMPIONS_API UMapConfigData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Config")
    TSubclassOf<class UMapCreator> MapCreatorClass;

    UPROPERTY(EditAnywhere, Category = "Config")
    int32 MapWidth;

    UPROPERTY(EditAnywhere, Category = "Config")
    int32 MapHeight;

    UPROPERTY(EditAnywhere, Category = "Config", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float AreaSpawnProbability;

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
