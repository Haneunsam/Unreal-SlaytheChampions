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
	 * 게임 최초 시작 시 스타터 덱으로 SaveGame 을 초기화.
	 * 이미 저장 파일이 있으면 아무것도 하지 않음.
	 * 덱 저장/로드는 UCardSaveGame ("PlayerDeckSave" 슬롯) 에서 전담.
	 *
	 * @param StarterDeckWarrior  DT_StarterDeck_Warrior
	 * @param StarterDeckMage     DT_StarterDeck_Mage
	 * @param StarterDeckHealer   DT_StarterDeck_Healer
	 */
	UFUNCTION(BlueprintCallable, Category = "SaveSystem|Deck")
	void InitDeckIfNew(UDataTable* StarterDeckWarrior, UDataTable* StarterDeckMage, UDataTable* StarterDeckHealer);
};
