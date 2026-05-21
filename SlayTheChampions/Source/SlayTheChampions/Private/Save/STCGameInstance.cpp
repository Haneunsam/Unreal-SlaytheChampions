// Fill out your copyright notice in the Description page of Project Settings.

#include "Save/STCGameInstance.h"
#include "Save/GameSaveSystem.h"
#include "Champion/ChampionInstance.h"
#include "Map/RunSystem.h"
#include "StarterDeckRow.h"
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
// 저장
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

	// 덱 데이터는 CachedSave->SavedPartyDecks 에 이미 최신 상태로 유지됨
	// (SaveDeckAfterBattle / AddCard / RemoveCard 호출 시마다 갱신)

	UGameplayStatics::SaveGameToSlot(Save, SaveSlotName, 0);
	UE_LOG(LogTemp, Log, TEXT("[STCGameInstance] SaveGameData completed"));
}

// ─────────────────────────────────────────────────────────────────────────────
// 로드
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

	UE_LOG(LogTemp, Log, TEXT("[STCGameInstance] LoadGameData completed - %d party decks"),
		CachedSave->SavedPartyDecks.Num());
}

// ─────────────────────────────────────────────────────────────────────────────
// 전투 종료 시 덱 저장 (A방식: DrawPile + Hand + DiscardPile 합산)
// ─────────────────────────────────────────────────────────────────────────────
void USTCGameInstance::SaveDeckAfterBattle(int32 PawnIndex,
	const TArray<FName>& DrawPile,
	const TArray<FName>& Hand,
	const TArray<FName>& DiscardPile)
{
	UGameSaveSystem* Save = EnsureCache(this, SaveSlotName, CachedSave);
	if (!Save) return;

	// PawnIndex 범위를 넘으면 배열 확장
	if (!Save->SavedPartyDecks.IsValidIndex(PawnIndex))
	{
		Save->SavedPartyDecks.SetNum(PawnIndex + 1);
	}

	// 세 더미를 합쳐서 저장
	TArray<FName> Combined;
	Combined.Append(DrawPile);
	Combined.Append(Hand);
	Combined.Append(DiscardPile);

	Save->SavedPartyDecks[PawnIndex].DeckCards = Combined;

	UGameplayStatics::SaveGameToSlot(Save, SaveSlotName, 0);
	UE_LOG(LogTemp, Log, TEXT("[STCGameInstance] Pawn%d deck saved after battle - %d cards"),
		PawnIndex, Combined.Num());
}

// ─────────────────────────────────────────────────────────────────────────────
// 파티원 덱 카드 목록 반환 (캐시에서 읽음)
// ─────────────────────────────────────────────────────────────────────────────
TArray<FName> USTCGameInstance::GetDeckCards(int32 PawnIndex) const
{
	if (!CachedSave || !CachedSave->SavedPartyDecks.IsValidIndex(PawnIndex))
		return {};

	return CachedSave->SavedPartyDecks[PawnIndex].DeckCards;
}

// ─────────────────────────────────────────────────────────────────────────────
// 카드 보상 시 추가
// ─────────────────────────────────────────────────────────────────────────────
void USTCGameInstance::AddCard(int32 PawnIndex, FName CardName)
{
	UGameSaveSystem* Save = EnsureCache(this, SaveSlotName, CachedSave);
	if (!Save) return;

	if (!Save->SavedPartyDecks.IsValidIndex(PawnIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("[STCGameInstance] AddCard: Invalid PawnIndex %d"), PawnIndex);
		return;
	}

	Save->SavedPartyDecks[PawnIndex].DeckCards.Add(CardName);
	UGameplayStatics::SaveGameToSlot(Save, SaveSlotName, 0);

	UE_LOG(LogTemp, Log, TEXT("[STCGameInstance] Pawn%d card added - %s (total %d)"),
		PawnIndex, *CardName.ToString(),
		Save->SavedPartyDecks[PawnIndex].DeckCards.Num());
}

// ─────────────────────────────────────────────────────────────────────────────
// 카드 제거
// ─────────────────────────────────────────────────────────────────────────────
void USTCGameInstance::RemoveCard(int32 PawnIndex, FName CardName)
{
	UGameSaveSystem* Save = EnsureCache(this, SaveSlotName, CachedSave);
	if (!Save) return;

	if (!Save->SavedPartyDecks.IsValidIndex(PawnIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("[STCGameInstance] RemoveCard: Invalid PawnIndex %d"), PawnIndex);
		return;
	}

	Save->SavedPartyDecks[PawnIndex].DeckCards.Remove(CardName);
	UGameplayStatics::SaveGameToSlot(Save, SaveSlotName, 0);

	UE_LOG(LogTemp, Log, TEXT("[STCGameInstance] Pawn%d card removed - %s"),
		PawnIndex, *CardName.ToString());
}

// ─────────────────────────────────────────────────────────────────────────────
// 게임 최초 시작 시 스타터 덱 초기화
// ─────────────────────────────────────────────────────────────────────────────
void USTCGameInstance::InitDeckIfNew(UDataTable* StarterDeckWarrior, UDataTable* StarterDeckMage, UDataTable* StarterDeckHealer)
{
	// 이미 저장 파일이 있고 덱 데이터가 있으면 초기화 스킵
	UGameSaveSystem* Save = EnsureCache(this, SaveSlotName, CachedSave);
	if (!Save) return;

	if (Save->SavedPartyDecks.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[STCGameInstance] InitDeckIfNew: Deck already exists - skip"));
		return;
	}

	// 파티원 3명 슬롯 초기화
	Save->SavedPartyDecks.SetNum(3);

	// Pawn0: Warrior
	Save->SavedPartyDecks[0].JobClass = EJobClass::Warrior;
	if (StarterDeckWarrior)
	{
		for (const FName& RowName : StarterDeckWarrior->GetRowNames())
		{
			if (const FStarterDeckRow* Row = StarterDeckWarrior->FindRow<FStarterDeckRow>(RowName, TEXT("InitDeck_Warrior")))
				Save->SavedPartyDecks[0].DeckCards.Add(Row->CardID);
		}
		UE_LOG(LogTemp, Log, TEXT("[STCGameInstance] Warrior deck initialized - %d cards"),
			Save->SavedPartyDecks[0].DeckCards.Num());
	}

	// Pawn1: Mage
	Save->SavedPartyDecks[1].JobClass = EJobClass::Mage;
	if (StarterDeckMage)
	{
		for (const FName& RowName : StarterDeckMage->GetRowNames())
		{
			if (const FStarterDeckRow* Row = StarterDeckMage->FindRow<FStarterDeckRow>(RowName, TEXT("InitDeck_Mage")))
				Save->SavedPartyDecks[1].DeckCards.Add(Row->CardID);
		}
		UE_LOG(LogTemp, Log, TEXT("[STCGameInstance] Mage deck initialized - %d cards"),
			Save->SavedPartyDecks[1].DeckCards.Num());
	}

	// Pawn2: Healer
	Save->SavedPartyDecks[2].JobClass = EJobClass::Healer;
	if (StarterDeckHealer)
	{
		for (const FName& RowName : StarterDeckHealer->GetRowNames())
		{
			if (const FStarterDeckRow* Row = StarterDeckHealer->FindRow<FStarterDeckRow>(RowName, TEXT("InitDeck_Healer")))
				Save->SavedPartyDecks[2].DeckCards.Add(Row->CardID);
		}
		UE_LOG(LogTemp, Log, TEXT("[STCGameInstance] Healer deck initialized - %d cards"),
			Save->SavedPartyDecks[2].DeckCards.Num());
	}

	UGameplayStatics::SaveGameToSlot(Save, SaveSlotName, 0);
	UE_LOG(LogTemp, Log, TEXT("[STCGameInstance] InitDeckIfNew completed"));
}
