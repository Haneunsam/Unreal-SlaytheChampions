// Fill out your copyright notice in the Description page of Project Settings.

#include "Save/STCGameInstance.h"
#include "Save/GameSaveSystem.h"
#include "Champion/ChampionInstance.h"
#include "Map/RunSystem.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

// ─────────────────────────────────────────────────────────────────────────────
// 내부 헬퍼: 캐시가 없으면 디스크에서 로드, 그것도 없으면 새로 생성
// ─────────────────────────────────────────────────────────────────────────────
static UGameSaveSystem* EnsureCache(USTCGameInstance* GI, const FString& SlotName, TObjectPtr<UGameSaveSystem>& CachedSave)
{
	if (CachedSave)
		return CachedSave;

	// 디스크에 저장 파일이 있으면 로드
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CachedSave = Cast<UGameSaveSystem>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		UE_LOG(LogTemp, Log, TEXT("[STCGameInstance] SaveGame loaded from disk"));
	}

	// 없으면 새 객체 생성
	if (!CachedSave)
	{
		CachedSave = Cast<UGameSaveSystem>(UGameplayStatics::CreateSaveGameObject(UGameSaveSystem::StaticClass()));
		UE_LOG(LogTemp, Log, TEXT("[STCGameInstance] SaveGame created (new)"));
	}

	return CachedSave;
}

// ─────────────────────────────────────────────────────────────────────────────
// 저장 (챔피언 + 맵 정보. 덱은 UCardSaveGame 에서 별도 저장)
// ─────────────────────────────────────────────────────────────────────────────
void USTCGameInstance::SaveGameData()
{
	UGameSaveSystem* Save = EnsureCache(this, SaveSlotName, CachedSave);
	if (!Save) return;

	// 챔피언 정보 수집
	if (UChampionInstance* PlayerSub = GetSubsystem<UChampionInstance>())
	{
		Save->SavedChampionsInfo = PlayerSub->GetChampionsInfo();
	}

	// 맵 정보 수집
	if (URunSystem* MapSub = GetSubsystem<URunSystem>())
	{
		Save->SavedMapInfo = MapSub->GetMapInfo();
	}

	UGameplayStatics::SaveGameToSlot(Save, SaveSlotName, 0);
	UE_LOG(LogTemp, Log, TEXT("[STCGameInstance] SaveGameData completed"));
}

// ─────────────────────────────────────────────────────────────────────────────
// 로드 (챔피언 + 맵 정보. 덱은 UCardSaveGame 에서 별도 로드)
// ─────────────────────────────────────────────────────────────────────────────
void USTCGameInstance::LoadGameData()
{
	if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		UE_LOG(LogTemp, Log, TEXT("[STCGameInstance] No save file found - skip load"));
		return;
	}

	CachedSave = Cast<UGameSaveSystem>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	if (!CachedSave)
	{
		UE_LOG(LogTemp, Warning, TEXT("[STCGameInstance] LoadGameData: Cast failed"));
		return;
	}

	// 챔피언 정보 배포
	if (UChampionInstance* PlayerSub = GetSubsystem<UChampionInstance>())
	{
		PlayerSub->SetChampionsInf(CachedSave->SavedChampionsInfo);
	}

	// 맵 정보 배포
	if (URunSystem* MapSub = GetSubsystem<URunSystem>())
	{
		MapSub->SetMapInfo(CachedSave->SavedMapInfo);
	}

	UE_LOG(LogTemp, Log, TEXT("[STCGameInstance] LoadGameData completed"));
}

// ─────────────────────────────────────────────────────────────────────────────
// 게임 최초 시작 시 스타터 덱 초기화
// 덱 저장/로드는 UCardSaveGame ("PlayerDeckSave" 슬롯) + UCardManager 에서 전담.
// 이 함수는 덱을 건드리지 않음 - 하위 호환용으로 남겨둠.
// ─────────────────────────────────────────────────────────────────────────────
void USTCGameInstance::InitDeckIfNew(UDataTable* StarterDeckWarrior, UDataTable* StarterDeckMage, UDataTable* StarterDeckHealer)
{
	// 덱 초기화는 UCardManager::InitializePartyDecks() 에서 처리.
	// 이 함수는 더 이상 덱을 직접 초기화하지 않음.
	UE_LOG(LogTemp, Log, TEXT("[STCGameInstance] InitDeckIfNew: Deck is managed by UCardManager - skipped"));
}
