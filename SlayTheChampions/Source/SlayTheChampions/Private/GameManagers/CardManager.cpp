#include "GameManagers/CardManager.h"
#include "Card/DeckComponent.h"
#include "Card/CardSubsystem.h"
#include "Card/CardSaveGame.h"

void UCardManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // 파티 최대 2명 슬롯 예약
    PartyDeckComponents.SetNum(2);

    // CardSubsystem 캐시
    CardSubsystem = GetGameInstance()->GetSubsystem<UCardSubsystem>();

    UE_LOG(LogTemp, Log, TEXT("[CardManager] Initialized."));
}

// ── 파티 덱 등록 ─────────────────────────────────────────────────────────────

void UCardManager::RegisterDeckComponent(int32 PawnIndex, UDeckComponent* InDeckComp)
{
    if (!IsValidPawnIndex(PawnIndex) || !InDeckComp)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[CardManager] RegisterDeckComponent: Invalid PawnIndex(%d) or null DeckComponent."),
            PawnIndex);
        return;
    }

    PartyDeckComponents[PawnIndex] = InDeckComp;

    UE_LOG(LogTemp, Log,
        TEXT("[CardManager] Pawn%d DeckComponent registered."), PawnIndex);
}

// ── 초기화 ───────────────────────────────────────────────────────────────────

void UCardManager::InitializePartyDecks()
{
    // 확정된 경로에서 StarterDeck DataTable 로드
    static const FSoftObjectPath WarriorPath(
        TEXT("/Game/01_Card/01_Test_DT/DT_StarterDeck_Warrior.DT_StarterDeck_Warrior"));
    static const FSoftObjectPath MagePath(
        TEXT("/Game/01_Card/01_Test_DT/DT_StarterDeck_Mage.DT_StarterDeck_Mage"));

    UDataTable* WarriorTable = Cast<UDataTable>(WarriorPath.TryLoad());
    UDataTable* MageTable    = Cast<UDataTable>(MagePath.TryLoad());

    if (!WarriorTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("[CardManager] DT_StarterDeck_Warrior not found at '%s'."),
            *WarriorPath.ToString());
    }
    if (!MageTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("[CardManager] DT_StarterDeck_Mage not found at '%s'."),
            *MagePath.ToString());
    }

    // SaveGame 있으면 로드, 없으면 StarterDeck 으로 새로 생성
    UCardSaveGame* Save = UCardSaveGame::LoadOrCreate(WarriorTable, MageTable);
    if (!Save)
    {
        UE_LOG(LogTemp, Error, TEXT("[CardManager] InitializePartyDecks: Failed to load or create SaveGame."));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[CardManager] Party decks initialized."));
}

// ── 저장 ─────────────────────────────────────────────────────────────────────

void UCardManager::SaveAllDecks()
{
    UCardSaveGame* Save = UCardSaveGame::LoadSave();
    if (!Save)
    {
        UE_LOG(LogTemp, Warning, TEXT("[CardManager] SaveAllDecks: No SaveGame found. Call InitializePartyDecks first."));
        return;
    }

    for (int32 i = 0; i < PartyDeckComponents.Num(); ++i)
    {
        UDeckComponent* DC = PartyDeckComponents[i];
        if (!DC) continue;

        // DrawPile + DiscardPile 합산 저장 (Hand는 전투 종료 시 이미 DiscardPile로 이동된 상태)
        TArray<FName> Combined;
        Combined.Append(DC->GetDrawPile());
        Combined.Append(DC->GetDiscardPile());

        if (Save->PartyDecks.IsValidIndex(i))
        {
            Save->PartyDecks[i].DeckCards = Combined;
            UE_LOG(LogTemp, Log,
                TEXT("[CardManager] Pawn%d deck saved - %d cards."), i, Combined.Num());
        }
    }

    UCardSaveGame::WriteSave(Save);
    UE_LOG(LogTemp, Log, TEXT("[CardManager] SaveAllDecks completed."));
}

// ── 보상 카드 ─────────────────────────────────────────────────────────────────

void UCardManager::AddRewardCard(int32 PawnIndex, FName CardName)
{
    if (!IsValidPawnIndex(PawnIndex))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[CardManager] AddRewardCard: Invalid PawnIndex(%d)."), PawnIndex);
        return;
    }

    UDeckComponent* DC = PartyDeckComponents[PawnIndex];
    if (!DC)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[CardManager] AddRewardCard: Pawn%d DeckComponent not registered."), PawnIndex);
        return;
    }

    // 메모리(DeckComponent) DiscardPile에 추가 (다음 RecycleDiscardIntoDraw 시 DrawPile로 합류)
    DC->DiscardCard(CardName);

    // SaveGame 에 반영
    UCardSaveGame::AddCard(PawnIndex, CardName);

    UE_LOG(LogTemp, Log,
        TEXT("[CardManager] Pawn%d reward card added - %s."), PawnIndex, *CardName.ToString());
}

// ── 조회 ─────────────────────────────────────────────────────────────────────

UDeckComponent* UCardManager::GetDeckComponent(int32 PawnIndex) const
{
    if (!IsValidPawnIndex(PawnIndex)) return nullptr;
    return PartyDeckComponents[PawnIndex];
}

const FCardDataRow* UCardManager::GetCardData(FName CardName) const
{
    if (!CardSubsystem) return nullptr;
    return CardSubsystem->GetCard(CardName);
}

// ── Private ──────────────────────────────────────────────────────────────────

bool UCardManager::IsValidPawnIndex(int32 PawnIndex) const
{
    return PartyDeckComponents.IsValidIndex(PawnIndex);
}
