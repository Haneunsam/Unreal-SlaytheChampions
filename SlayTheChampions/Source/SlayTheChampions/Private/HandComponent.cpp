#include "HandComponent.h"
#include "Algo/RandomShuffle.h"

UHandComponent::UHandComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

// ── 초기화 ────────────────────────────────────────────────────────────────────

void UHandComponent::InitializeDeck(const TArray<FName>& InDeckNames)
{
    DrawPile = InDeckNames;
    Hand.Reset();
    DiscardPile.Reset();

    // 초기 셔플
    Algo::RandomShuffle(DrawPile);

    BroadcastHandChanged();
}

// ── 드로우 ────────────────────────────────────────────────────────────────────

void UHandComponent::DrawStartOfTurn()
{
    DrawCards(DefaultDrawCount);
}

TArray<FName> UHandComponent::DrawCards(int32 Count)
{
    TArray<FName> Drawn;

    for (int32 i = 0; i < Count; ++i)
    {
        // DrawPile 이 비었으면 DiscardPile 재순환
        if (DrawPile.IsEmpty())
        {
            if (DiscardPile.IsEmpty()) break;  // 둘 다 비었으면 드로우 불가
            RecycleDiscardIntoDraw();
        }

        FName Card = DrawPile.Pop();
        Hand.Add(Card);
        Drawn.Add(Card);

        OnCardDrawn.Broadcast(Card);
    }

    if (!Drawn.IsEmpty())
    {
        BroadcastHandChanged();
    }

    return Drawn;
}

// ── 카드 사용 ─────────────────────────────────────────────────────────────────

bool UHandComponent::PlayCard(FName CardName)
{
    const int32 Idx = Hand.IndexOfByKey(CardName);
    if (Idx == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("UHandComponent::PlayCard ? '%s' not found in hand."),
            *CardName.ToString());
        return false;
    }

    Hand.RemoveAt(Idx);
    DiscardPile.Add(CardName);

    OnCardPlayed.Broadcast(CardName);
    BroadcastHandChanged();
    return true;
}

// ── 턴 종료 ───────────────────────────────────────────────────────────────────

void UHandComponent::DiscardHand()
{
    DiscardPile.Append(Hand);
    Hand.Reset();
    BroadcastHandChanged();
}

// ── Private ───────────────────────────────────────────────────────────────────

void UHandComponent::RecycleDiscardIntoDraw()
{
    DrawPile = MoveTemp(DiscardPile);
    DiscardPile.Reset();
    Algo::RandomShuffle(DrawPile);

    UE_LOG(LogTemp, Log,
        TEXT("UHandComponent: DiscardPile recycled → DrawPile (%d cards, reshuffled)."),
        DrawPile.Num());
}

void UHandComponent::BroadcastHandChanged()
{
    OnHandChanged.Broadcast(Hand);
}