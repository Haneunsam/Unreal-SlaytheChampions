// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Champion/ChampionStruct.h"
#include "Map/MapStruct.h"
#include "GameSaveSystem.generated.h"

/**
 * UGameSaveSystem
 *
 * 게임 전체 세이브 데이터를 담는 SaveGame 클래스.
 * STCGameInstance::SaveGameData / LoadGameData 를 통해 저장/로드.
 * 슬롯명: "SaveData_Slot0"
 */
UCLASS()
class SLAYTHECHAMPIONS_API UGameSaveSystem : public USaveGame
{
	GENERATED_BODY()

public:
	/* 플레이어 챔피언 정보 */
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	FSaveChampionsInfo SavedChampionsInfo;

	/* Map관련 저장 */
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	FSaveMapInfo SavedMapInfo;

	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	int32 SavedCurrentNodeIndex;

	// 덱 데이터는 UCardSaveGame ("PlayerDeckSave" 슬롯) 에서 별도 관리
};
