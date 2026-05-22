#include "Card/HandComponent.h"
#include "Card/DeckComponent.h"

UHandComponent::UHandComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

// ── 초기화 ───────────────────────────────────────────────────────────────────

// begine 보단 빠르지만 begine 느낌
void UHandComponent::InitializeDeck(UDeckComponent* InDeckComponent, const TArray<FName>& InDeckNames)
{
    // DeckComponent 참조 저장
    DeckComponent = InDeckComponent;

    if (!DeckComponent)
    {
        UE_LOG(LogTemp, Error,
            TEXT("UHandComponent::InitializeDeck - DeckComponent is null. Deck not initialized."));
        return;
    }

    // 파일 셔플/초기화는 DeckComponent 에 위임, Hand는 빈 상태로 반환
    DeckComponent->InitializeDeck(InDeckNames, Hand);

    BroadcastHandChanged();
}

// ── 드로우 ───────────────────────────────────────────────────────────────────

void UHandComponent::DrawStartOfTurn()
{
    DrawCards(DefaultDrawCount);
}

TArray<FName> UHandComponent::DrawCards(int32 Count)
{
    if (!DeckComponent)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("UHandComponent::DrawCards - DeckComponent is null. Call InitializeDeck first."));
        return {};
    }

    // 드로우 및 Over-Draw 처리를 DeckComponent 에 위임
    // Over-Draw 발생 시 DeckComponent::OnOverDraw 가 브로드캐스트됨
    TArray<FName> Drawn = DeckComponent->DrawCards(Count, Hand);

    for (const FName& Card : Drawn)
    {
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
            TEXT("UHandComponent::PlayCard - '%s' not found in hand."),
            *CardName.ToString());
        return false;
    }

    Hand.RemoveAt(Idx);

    // DiscardPile 이동은 DeckComponent 에 위임
    if (DeckComponent)
    {
        DeckComponent->DiscardCard(CardName);
    }

    OnCardPlayed.Broadcast(CardName);
    BroadcastHandChanged();
    return true;
}

// ── 턴 종료 ───────────────────────────────────────────────────────────────────

void UHandComponent::DiscardHand()
{
    if (!DeckComponent)
    {
        // DeckComponent 없이도 Hand는 비워줌
        Hand.Reset(); // Reset()함수는 언리얼에 기본으로 내장되어 있는 함수로 
        BroadcastHandChanged();
        return;
    }

    // Hand 전체를 DiscardPile 로 이동 (Hand 초기화 포함)
    DeckComponent->DiscardAll(Hand);

    BroadcastHandChanged();
}

// ── Private ──────────────────────────────────────────────────────────────────

void UHandComponent::BroadcastHandChanged()
{
    OnHandChanged.Broadcast(Hand);
}
