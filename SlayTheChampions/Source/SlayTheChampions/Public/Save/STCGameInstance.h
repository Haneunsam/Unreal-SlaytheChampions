// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Save/GameSaveSystem.h"
#include "STCGameInstance.generated.h"

/**
 * USTCGameInstance
 *
 * 게임 전체 저장/로드를 담당하는 GameInstance.
 * SaveGameData / LoadGameData 로 챔피언 + 맵 + 덱 데이터를 한 슬롯에 저장.
 * 덱 데이터는 CachedSave 에 캐싱되어 매번 디스크 I/O 없이 접근 가능.
 */
UCLASS()
class SLAYTHECHAMPIONS_API USTCGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	/* 세이브 슬롯 이름 */
	const FString SaveSlotName = TEXT("SaveData_Slot0");

	/* 런 동안 메모리에 캐싱된 SaveGame 객체 (디스크 I/O 최소화용) */
	UPROPERTY()
	TObjectPtr<UGameSaveSystem> CachedSave;

public:
	/* 챔피언 + 맵 + 덱 데이터를 한 번에 저장 */
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SaveGameData();

	/* 저장된 데이터를 불러와 각 서브시스템에 배포 */
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void LoadGameData();

	/**
	 * 전투 종료 시 파티원의 덱 상태를 캐시에 저장하고 디스크에 기록.
	 * DrawPile + Hand + DiscardPile 을 합쳐서 저장 (A방식).
	 *
	 * @param PawnIndex    파티원 인덱스 (0 = Pawn1, 1 = Pawn2, 2 = Pawn3)
	 * @param DrawPile     현재 드로우 더미
	 * @param Hand         현재 손패
	 * @param DiscardPile  현재 버리기 더미
	 */
	UFUNCTION(BlueprintCallable, Category = "SaveSystem|Deck")
	void SaveDeckAfterBattle(int32 PawnIndex,
		const TArray<FName>& DrawPile,
		const TArray<FName>& Hand,
		const TArray<FName>& DiscardPile);

	/**
	 * 파티원의 저장된 덱 카드 목록을 반환.
	 * LoadGameData 이후 캐시에서 바로 읽음 (디스크 접근 없음).
	 *
	 * @param PawnIndex  파티원 인덱스
	 * @return 카드 ID 배열 (저장 데이터 없으면 빈 배열)
	 */
	UFUNCTION(BlueprintCallable, Category = "SaveSystem|Deck")
	TArray<FName> GetDeckCards(int32 PawnIndex) const;

	/**
	 * 카드 보상 시 덱에 카드를 추가하고 즉시 디스크에 저장.
	 *
	 * @param PawnIndex  파티원 인덱스
	 * @param CardName   추가할 카드 ID (DT_Cards RowName)
	 */
	UFUNCTION(BlueprintCallable, Category = "SaveSystem|Deck")
	void AddCard(int32 PawnIndex, FName CardName);

	/**
	 * 카드 제거 시 덱에서 카드를 삭제하고 즉시 디스크에 저장.
	 *
	 * @param PawnIndex  파티원 인덱스
	 * @param CardName   제거할 카드 ID
	 */
	UFUNCTION(BlueprintCallable, Category = "SaveSystem|Deck")
	void RemoveCard(int32 PawnIndex, FName CardName);

	/**
	 * 게임 최초 시작 시 스타터 덱으로 SaveGame 을 초기화.
	 * 이미 저장 파일이 있으면 아무것도 하지 않음.
	 *
	 * @param StarterDeckWarrior  DT_StarterDeck_Warrior
	 * @param StarterDeckMage     DT_StarterDeck_Mage
	 * @param StarterDeckHealer   DT_StarterDeck_Healer
	 */
	UFUNCTION(BlueprintCallable, Category = "SaveSystem|Deck")
	void InitDeckIfNew(UDataTable* StarterDeckWarrior, UDataTable* StarterDeckMage, UDataTable* StarterDeckHealer);
};
