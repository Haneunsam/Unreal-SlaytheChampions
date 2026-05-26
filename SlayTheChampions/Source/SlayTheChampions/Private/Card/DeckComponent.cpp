#include "Card/DeckComponent.h"
#include "Algo/RandomShuffle.h"

UDeckComponent::UDeckComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

// ── 초기화 ───────────────────────────────────────────────────────────────────

void UDeckComponent::InitializeDeck(const TArray<FName>& InDeckNames, TArray<FName>& OutHand)
{
    // DrawPile 에 전달받은 카드 목록을 복사
    DrawPile = InDeckNames;

    // 버리기/소멸 파일 초기화
    // ExhaustPile 초기화: 이전 전투에서 소멸된 카드들이 InDeckNames 에 포함되어 이미 복귀한 상태 (A방식)
    DiscardPile.Reset();
    ExhaustPile.Reset();

    // Hand 초기화 (전투 시작 시 빈 손패)
    OutHand.Reset();

    // Fisher-Yates 셔플로 DrawPile 무작위 정렬 (Algo::RandomShuffle 내부 구현)
    Algo::RandomShuffle(DrawPile);

    UE_LOG(LogTemp, Warning,
        TEXT("UDeckComponent: Deck initialized. DrawPile=%d cards."),
        DrawPile.Num());
}

// ── 드로우 ───────────────────────────────────────────────────────────────────

TArray<FName> UDeckComponent::DrawCards(int32 Count, TArray<FName>& InOutHand)
{
    TArray<FName> DrawnToHand;

    for (int32 i = 0; i < Count; ++i)
    {
        // DrawPile 소진 시 DiscardPile 을 재활용해 DrawPile 재생성
        if (DrawPile.IsEmpty())
        {
            if (DiscardPile.IsEmpty())
            {
                // DrawPile, DiscardPile 모두 비어있으면 드로우 불가
                UE_LOG(LogTemp, Warning,
                    TEXT("UDeckComponent: DrawCards - No cards left to draw."));
                break;
            }
            RecycleDiscardIntoDraw();
        }

        FName Card = DrawPile.Pop();

        // Over-Draw 처리: Hand 가 MaxHandSize 이상이면 뽑은 카드를 DiscardPile 에 직행
        if (InOutHand.Num() >= MaxHandSize)
        {
            DiscardPile.Add(Card);
            OnOverDraw.Broadcast(Card);

            UE_LOG(LogTemp, Warning,
                TEXT("UDeckComponent: Over-Draw! '%s' sent directly to DiscardPile. Hand is full (%d/%d)."),
                *Card.ToString(), InOutHand.Num(), MaxHandSize);
            continue;
        }

        // 정상 드로우: Hand 와 반환 배열에 추가
        InOutHand.Add(Card);
        DrawnToHand.Add(Card);
    }

    return DrawnToHand;
}

// ── 버리기 ───────────────────────────────────────────────────────────────────

void UDeckComponent::DiscardAll(TArray<FName>& InOutHand)
{
    // Hand 전체를 DiscardPile 로 이동 후 Hand 비우기 (턴 종료 시 호출)
    DiscardPile.Append(InOutHand);
    InOutHand.Reset();

    UE_LOG(LogTemp, Warning,
        TEXT("UDeckComponent: Hand discarded. DiscardPile=%d cards."),
        DiscardPile.Num());
}

void UDeckComponent::DiscardCard(FName CardName)
{
    // 개별 카드 하나를 DiscardPile 에 추가 (Hand 제거는 CardUserComponent 에서 처리)
    DiscardPile.Add(CardName);
}

// ── 소멸 ─────────────────────────────────────────────────────────────────────

void UDeckComponent::ExhaustCard(FName CardName)
{
    // 소멸 카드를 ExhaustPile 에 추가하고 이벤트 브로드캐스트
    // ExhaustPile 은 SaveAllDecks 에 포함되지 않으며, 다음 전투 시작 시 덱에 복귀 (A방식)
    ExhaustPile.Add(CardName);
    OnCardExhausted.Broadcast(CardName);

    UE_LOG(LogTemp, Warning,
        TEXT("UDeckComponent: '%s' exhausted. ExhaustPile=%d cards."),
        *CardName.ToString(), ExhaustPile.Num());
}

// ── Private ──────────────────────────────────────────────────────────────────

void UDeckComponent::RecycleDiscardIntoDraw()
{
    // DiscardPile 을 DrawPile 로 이동 (MoveTemp 로 복사 없이 처리)
    DrawPile = MoveTemp(DiscardPile);
    DiscardPile.Reset();

    // Fisher-Yates 셔플로 재활용된 덱을 다시 섞음
    Algo::RandomShuffle(DrawPile);

    OnDeckRecycled.Broadcast();

    UE_LOG(LogTemp, Warning,
        TEXT("UDeckComponent: DiscardPile recycled into DrawPile. %d cards reshuffled."),
        DrawPile.Num());
}
