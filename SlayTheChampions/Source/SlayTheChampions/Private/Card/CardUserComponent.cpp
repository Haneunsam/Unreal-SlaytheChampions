#include "Card/CardUserComponent.h"
#include "Card/DeckComponent.h"
#include "Card/CardSubsystem.h"
#include "Card/CardSaveGame.h"
#include "Card/CardDataTypes.h"
#include "GameManagers/CardManager.h"
#include "Engine/GameInstance.h"

UCardUserComponent::UCardUserComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

// ── Lifecycle ────────────────────────────────────────────────────────────────

void UCardUserComponent::BeginPlay()
{
    Super::BeginPlay();

    // DeckComponent 를 Owner 에 동적 생성 후 등록
    DeckComponent = NewObject<UDeckComponent>(GetOwner(), UDeckComponent::StaticClass(), TEXT("DeckComponent"));
    DeckComponent->RegisterComponent();

    // CardManager 에 DeckComponent 등록
    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        if (UCardManager* CM = GI->GetSubsystem<UCardManager>())
        {
            CM->RegisterDeckComponent(PawnIndex, DeckComponent);
        }
    }

    // 덱 자동 초기화
    InitializeDeck();
}

// ── 덱 초기화 / 저장 ─────────────────────────────────────────────────────────

void UCardUserComponent::InitializeDeck()
{
    if (!DeckComponent) return;

    TArray<FName> DeckNames;

    // 1순위: SaveGame 에서 덱 불러오기
    TArray<FName> SavedDeck = UCardSaveGame::GetDeckCards(PawnIndex);
    if (SavedDeck.Num() > 0)
    {
        DeckNames = SavedDeck;
        UE_LOG(LogTemp, Warning,
            TEXT("[CardUserComponent] Pawn%d - Loaded from SaveGame (%d cards)."),
            PawnIndex, DeckNames.Num());
    }

    // 2순위: OverrideDeckNames 직접 지정 (테스트용)
    if (DeckNames.IsEmpty() && OverrideDeckNames.Num() > 0)
    {
        DeckNames = OverrideDeckNames;
        UE_LOG(LogTemp, Warning,
            TEXT("[CardUserComponent] Pawn%d - Using OverrideDeck (%d cards)."),
            PawnIndex, DeckNames.Num());
    }

    // 3순위: CardSubsystem 에서 직업 기반 자동 조회
    if (DeckNames.IsEmpty())
    {
        if (UGameInstance* GI = GetWorld()->GetGameInstance())
        {
            if (UCardSubsystem* CS = GI->GetSubsystem<UCardSubsystem>())
            {
                DeckNames = CS->GetCardNamesByClass(JobClass);
                UE_LOG(LogTemp, Warning,
                    TEXT("[CardUserComponent] Pawn%d - Loaded from CardSubsystem (%d cards)."),
                    PawnIndex, DeckNames.Num());
            }
        }
    }

    if (DeckNames.IsEmpty())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[CardUserComponent] Pawn%d - No cards found. Deck not initialized."),
            PawnIndex);
        return;
    }

    // DeckComponent 초기화 (셔플 포함), Hand 는 빈 상태로 시작
    DeckComponent->InitializeDeck(DeckNames, Hand);
    UE_LOG(LogTemp, Warning,
        TEXT("[CardUserComponent] Pawn%d - Deck initialized (%d cards)."),
        PawnIndex, DeckNames.Num());
}

void UCardUserComponent::SaveDeckToSaveGame()
{
    if (!DeckComponent) return;

    // DrawPile + Hand + DiscardPile 합산 저장 (ExhaustPile 제외)
    UCardSaveGame::SaveDeckAfterBattle(
        PawnIndex,
        DeckComponent->GetDrawPile(),
        Hand,
        DeckComponent->GetDiscardPile()
    );

    UE_LOG(LogTemp, Log,
        TEXT("[CardUserComponent] Pawn%d - Deck saved to SaveGame."), PawnIndex);
}

// ── 턴 조작 ──────────────────────────────────────────────────────────────────

void UCardUserComponent::DrawStartOfTurn()
{
    DrawCards(DefaultDrawCount);
}

void UCardUserComponent::DrawCards(int32 Count)
{
    if (!DeckComponent) return;

    TArray<FName> Drawn = DeckComponent->DrawCards(Count, Hand);

    if (!Drawn.IsEmpty())
    {
        BroadcastHandChanged();
    }
}

bool UCardUserComponent::PlayCard(FName CardName)
{
    const int32 Idx = Hand.IndexOfByKey(CardName);
    if (Idx == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[CardUserComponent] PlayCard - '%s' not found in hand."),
            *CardName.ToString());
        return false;
    }

    Hand.RemoveAt(Idx);

    if (DeckComponent)
    {
        // 소멸 카드(bExhaust)이면 ExhaustPile, 아니면 DiscardPile
        if (IsExhaustCard(CardName))
        {
            DeckComponent->ExhaustCard(CardName);
        }
        else
        {
            DeckComponent->DiscardCard(CardName);
        }
    }

    OnCardPlayed.Broadcast(CardName);
    BroadcastHandChanged();
    return true;
}

void UCardUserComponent::DiscardHand()
{
    if (!DeckComponent) return;

    // Hand 전체를 DiscardPile 로 이동
    DeckComponent->DiscardAll(Hand);

    BroadcastHandChanged();
}

// ── 조회 ─────────────────────────────────────────────────────────────────────

int32 UCardUserComponent::GetDrawPileCount() const
{
    return DeckComponent ? DeckComponent->GetDrawPileCount() : 0;
}

int32 UCardUserComponent::GetDiscardPileCount() const
{
    return DeckComponent ? DeckComponent->GetDiscardPileCount() : 0;
}

int32 UCardUserComponent::GetExhaustPileCount() const
{
    return DeckComponent ? DeckComponent->GetExhaustPileCount() : 0;
}

// ── Private ──────────────────────────────────────────────────────────────────

void UCardUserComponent::BroadcastHandChanged()
{
    OnHandChanged.Broadcast(Hand);
}

bool UCardUserComponent::IsExhaustCard(FName CardName) const
{
    if (UGameInstance* GI = GetWorld()->GetGameInstance())
    {
        if (UCardSubsystem* CS = GI->GetSubsystem<UCardSubsystem>())
        {
            const FCardDataRow* Row = CS->GetCard(CardName);
            return Row && Row->bExhaust;
        }
    }
    return false;
}
