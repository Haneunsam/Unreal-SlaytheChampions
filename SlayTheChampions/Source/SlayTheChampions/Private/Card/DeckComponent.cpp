#include "Card/DeckComponent.h"
#include "Algo/RandomShuffle.h"

UDeckComponent::UDeckComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

// ── 초기화 ───────────────────────────────────────────────────────────────────

void UDeckComponent::InitializeDeck(const TArray<FName>& InDeckNames, TArray<FName>& OutHand)
{
    DrawPile = InDeckNames;
    DiscardPile.Reset();
    OutHand.Reset();

    // 초기 셔플 (Fisher-Yates, Algo::RandomShuffle 내부 구현)
    Algo::RandomShuffle(DrawPile);

    UE_LOG(LogTemp, Log,
        TEXT("UDeckComponent: Deck initialized. DrawPile=%d cards."),
        DrawPile.Num());
}

// ── 드로우 ───────────────────────────────────────────────────────────────────

TArray<FName> UDeckComponent::DrawCards(int32 Count, TArray<FName>& InOutHand)
{
    TArray<FName> DrawnToHand;

    for (int32 i = 0; i < Count; ++i)
    {
        // DrawPile 소진 시 DiscardPile 재활용
        if (DrawPile.IsEmpty())
        {
            if (DiscardPile.IsEmpty())
            {
                // 뽑을 카드가 아예 없음
                UE_LOG(LogTemp, Log,
                    TEXT("UDeckComponent: DrawCards - No cards left to draw."));
                break;
            }
            RecycleDiscardIntoDraw();
        }

        FName Card = DrawPile.Pop();

        // Over-Draw 처리: Hand가 꽉 찼으면 DiscardPile 직행 (CLAUDE.md 5.4 규칙 1)
        if (InOutHand.Num() >= MaxHandSize)
        {
            DiscardPile.Add(Card);
            OnOverDraw.Broadcast(Card);

            UE_LOG(LogTemp, Warning,
                TEXT("UDeckComponent: Over-Draw! '%s' sent directly to DiscardPile. Hand is full (%d/%d)."),
                *Card.ToString(), InOutHand.Num(), MaxHandSize);
            continue;
        }

        InOutHand.Add(Card);
        DrawnToHand.Add(Card);
    }

    return DrawnToHand;
}

// ── 버리기 ───────────────────────────────────────────────────────────────────

void UDeckComponent::DiscardAll(TArray<FName>& InOutHand)
{
    DiscardPile.Append(InOutHand);
    InOutHand.Reset();

    UE_LOG(LogTemp, Log,
        TEXT("UDeckComponent: Hand discarded. DiscardPile=%d cards."),
        DiscardPile.Num());
}

void UDeckComponent::DiscardCard(FName CardName)
{
    DiscardPile.Add(CardName);
}

// ── Private ──────────────────────────────────────────────────────────────────

void UDeckComponent::RecycleDiscardIntoDraw()
{
    DrawPile = MoveTemp(DiscardPile);
    DiscardPile.Reset();

    // Fisher-Yates 셔플로 재활용된 덱을 섞음
    Algo::RandomShuffle(DrawPile);

    OnDeckRecycled.Broadcast();

    UE_LOG(LogTemp, Log,
        TEXT("UDeckComponent: DiscardPile recycled into DrawPile. %d cards reshuffled."),
        DrawPile.Num());
}
