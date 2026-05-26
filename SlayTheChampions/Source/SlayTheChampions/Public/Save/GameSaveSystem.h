// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Champion/ChampionStruct.h"
#include "Map/MapStruct.h"
#include "Card/CardDataTypes.h"
#include "GameSaveSystem.generated.h"

/**
 * FPlayerDeckSaveData
 *
 * 파티원 1명의 덱 데이터를 저장하는 구조체.
 * UGameSaveSystem 안에 파티원 수만큼 배열로 저장된다.
 * Index 0 = Pawn1(Warrior), Index 1 = Pawn2(Mage), Index 2 = Pawn3(Healer)
 */
USTRUCT(BlueprintType)
struct FPlayerDeckSaveData
{
    GENERATED_BODY()

    // 이 파티원의 직업 (Warrior, Mage, Healer 등)
    UPROPERTY(BlueprintReadWrite, Category = "SaveData")
    EJobClass JobClass = EJobClass::Warrior;

    /**
     * 카드 ID 배열.
     * DT_Cards 의 RowName 기준 (예: Warrior_Attack, Warrior_Defend)
     * 전투 종료 시 DrawPile + Hand + DiscardPile 을 합쳐서 저장.
     * 전투 시작 시 이 배열을 셔플해서 DrawPile 로 사용.
     */
    UPROPERTY(BlueprintReadWrite, Category = "SaveData")
    TArray<FName> DeckCards;
};

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

	/* 파티원별 덱 데이터 배열 (Index 0 = Pawn1, Index 1 = Pawn2, Index 2 = Pawn3) */
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	TArray<FPlayerDeckSaveData> SavedPartyDecks;
};
