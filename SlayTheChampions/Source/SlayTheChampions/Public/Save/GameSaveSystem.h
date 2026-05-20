// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Champion/ChampionStruct.h"
#include "Map/MapStruct.h"
#include "GameSaveSystem.generated.h"

/**
 * 
 */
UCLASS()
class SLAYTHECHAMPIONS_API UGameSaveSystem : public USaveGame
{
	GENERATED_BODY()
	
public:
	/*플레이어 저장 정보*/
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	FSaveChampionsInfo SavedChampionsInfo;

	/*Map저장 정보*/
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	FSaveMapInfo SavedMapInfo; 

	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	int32 SavedCurrentNodeIndex;
};
