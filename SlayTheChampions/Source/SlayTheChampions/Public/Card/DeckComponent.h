#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DeckComponent.generated.h"

// Over-Draw 발생 시 (Hand가 꽉 찬 상태에서 뽑으려 할 때) 브로드캐스트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOverDraw, FName, OverdrawnCard);

// DiscardPile이 DrawPile로 재활용(셔플)될 때 브로드캐스트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeckRecycled);

/**
 * UDeckComponent
 *
 * DrawPile / DiscardPile 의 상태 전환, 셔플, 드로우를 전담하는 컴포넌트.
 * CLAUDE.md 5.2 규칙: 로직/타겟팅은 이 컴포넌트 밖에서 처리한다.
 *
 * 역할:
 *  - 덱 초기화 및 셔플 (Fisher-Yates / Algo::RandomShuffle)
 *  - DrawPile -> Hand 드로우 (Over-Draw 처리 포함)
 *  - Hand / 개별 카드 -> DiscardPile 이동
 *  - DrawPile 소진 시 DiscardPile 자동 재활용
 *
 * 금지 사항 (CLAUDE.md 5.2):
 *  - 카드 플레이 가능 여부 판단 (UCardPlayableComponent 담당)
 *  - 타겟 선택/검증 로직
 *  - 액터 스탯 직접 수정
 */
UCLASS(ClassGroup = "Card", meta = (BlueprintSpawnableComponent))
class SLAYTHECHAMPIONS_API UDeckComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDeckComponent();

    // ── 이벤트 델리게이트 ────────────────────────────────────────────────────

    // Hand가 꽉 찬 상태(>= MaxHandSize)에서 드로우 시 발생
    UPROPERTY(BlueprintAssignable, Category = "Card|Deck")
    FOnOverDraw OnOverDraw;

    // DiscardPile -> DrawPile 재활용 완료 시 발생
    UPROPERTY(BlueprintAssignable, Category = "Card|Deck")
    FOnDeckRecycled OnDeckRecycled;

    // ── 설정 ─────────────────────────────────────────────────────────────────

    // Hand 최대 보유 장수. 초과 시 Over-Draw 처리 (CLAUDE.md 5.4 규칙 1)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card|Deck",
        meta = (ClampMin = "1"))
    int32 MaxHandSize = 10;

    // ── 초기화 ───────────────────────────────────────────────────────────────

    /**
     * 덱 전체를 초기화한다.
     * DrawPile = InDeckNames 셔플, Hand/DiscardPile 초기화.
     * @param InDeckNames  카드 ID(DT_Cards RowName) 배열
     * @param OutHand      초기화 후 Hand 배열 (빈 상태)
     */
    UFUNCTION(BlueprintCallable, Category = "Card|Deck")
    void InitializeDeck(const TArray<FName>& InDeckNames, TArray<FName>& OutHand);

    // ── 드로우 ───────────────────────────────────────────────────────────────

    /**
     * Count 장만큼 드로우한다.
     * - Hand가 MaxHandSize 이상이면 Over-Draw: 뽑은 카드를 DiscardPile에 직행시키고
     *   OnOverDraw 브로드캐스트 (CLAUDE.md 5.4 규칙 1).
     * - DrawPile 소진 시 DiscardPile을 자동 재활용.
     * @param Count    드로우 장 수
     * @param InOutHand  현재 Hand 배열 (in/out). 드로우된 카드가 추가됨.
     * @return 실제로 Hand에 추가된 카드 목록
     */
    UFUNCTION(BlueprintCallable, Category = "Card|Deck")
    TArray<FName> DrawCards(int32 Count, TArray<FName>& InOutHand);

    // ── 버리기 ───────────────────────────────────────────────────────────────

    /**
     * Hand 전체를 DiscardPile로 이동한다. Hand는 비워진다.
     * @param InOutHand  현재 Hand 배열 (in/out). 호출 후 빈 상태.
     */
    UFUNCTION(BlueprintCallable, Category = "Card|Deck")
    void DiscardAll(TArray<FName>& InOutHand);

    /**
     * 개별 카드 하나를 DiscardPile로 이동한다.
     * Hand에서 카드를 제거하는 것은 UHandComponent 책임이므로,
     * 여기서는 DiscardPile에 추가만 한다.
     * @param CardName  버릴 카드 ID
     */
    UFUNCTION(BlueprintCallable, Category = "Card|Deck")
    void DiscardCard(FName CardName);

    // ── 조회 ─────────────────────────────────────────────────────────────────

    UFUNCTION(BlueprintPure, Category = "Card|Deck")
    const TArray<FName>& GetDrawPile() const { return DrawPile; }

    UFUNCTION(BlueprintPure, Category = "Card|Deck")
    const TArray<FName>& GetDiscardPile() const { return DiscardPile; }

    UFUNCTION(BlueprintPure, Category = "Card|Deck")
    int32 GetDrawPileCount() const { return DrawPile.Num(); }

    UFUNCTION(BlueprintPure, Category = "Card|Deck")
    int32 GetDiscardPileCount() const { return DiscardPile.Num(); }

private:
    // ── 파일 상태 배열 (GC 안전) ─────────────────────────────────────────────

    UPROPERTY()
    TArray<FName> DrawPile;

    UPROPERTY()
    TArray<FName> DiscardPile;

    // DiscardPile -> DrawPile 재활용 + 셔플 (Fisher-Yates / Algo::RandomShuffle)
    void RecycleDiscardIntoDraw();
};
