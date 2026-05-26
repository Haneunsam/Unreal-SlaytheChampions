#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Card/CardDataTypes.h"
#include "HandComponent.generated.h"

class UDeckComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHandChanged, const TArray<FName>&, CurrentHand);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardDrawn, FName, DrawnCard);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardPlayed, FName, PlayedCard);

/**
 * UHandComponent
 *
 * 파티원 1명의 Hand(패) 배열을 관리하는 컴포넌트.
 * 특정 Actor 에 종속되지 않도록 ActorComponent 로 설계.
 * Enemy 측에는 사용 안 함.
 *
 * 역할 (CLAUDE.md 5.2):
 *  - Hand 배열 단독 소유
 *  - 드로우/버리기 요청을 UDeckComponent 에 위임
 *  - 카드 사용 -> Hand에서 제거 -> UDeckComponent::DiscardCard 호출
 *  - 이벤트 델리게이트 발행 (UI/Blueprint 연동)
 *
 * 금지 사항:
 *  - DrawPile / DiscardPile 직접 보유 (UDeckComponent 담당)
 *  - 카드 효과 실행 (UActionQueueComponent 담당)
 */
UCLASS(ClassGroup = "Card", meta = (BlueprintSpawnableComponent))
class SLAYTHECHAMPIONS_API UHandComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHandComponent();

    // ── 이벤트 델리게이트 ────────────────────────────────────────────────────

    UPROPERTY(BlueprintAssignable, Category = "Card|Hand")
    FOnHandChanged OnHandChanged;

    UPROPERTY(BlueprintAssignable, Category = "Card|Hand")
    FOnCardDrawn OnCardDrawn;

    UPROPERTY(BlueprintAssignable, Category = "Card|Hand")
    FOnCardPlayed OnCardPlayed;

    // ── 설정 ─────────────────────────────────────────────────────────────────

    // 턴 시작 기본 드로우 수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card|Hand",
        meta = (ClampMin = "1"))
    int32 DefaultDrawCount = 5;

    // ── 초기화 ───────────────────────────────────────────────────────────────

    /**
     * 덱을 초기화하고 Hand를 빈 상태로 준비한다.
     * 실제 파일 셔플은 DeckComponent 에 위임.
     * @param InDeckComponent  같은 Actor 에 붙어있는 UDeckComponent
     * @param InDeckNames      카드 ID 배열
     */
    UFUNCTION(BlueprintCallable, Category = "Card|Hand")
    void InitializeDeck(UDeckComponent* InDeckComponent, const TArray<FName>& InDeckNames);

    // ── 드로우 ───────────────────────────────────────────────────────────────

    // DefaultDrawCount 만큼 드로우 (DeckComponent 에 위임)
    UFUNCTION(BlueprintCallable, Category = "Card|Hand")
    void DrawStartOfTurn();

    // N장 드로우 (DeckComponent 에 위임). 실제 Hand에 추가된 카드만 반환.
    UFUNCTION(BlueprintCallable, Category = "Card|Hand")
    TArray<FName> DrawCards(int32 Count);

    // ── 카드 사용 ─────────────────────────────────────────────────────────────

    /**
     * Hand 에서 카드를 사용한다.
     * Hand 제거 후 DeckComponent::DiscardCard 호출.
     * Hand 에 없으면 false 반환.
     */
    UFUNCTION(BlueprintCallable, Category = "Card|Hand")
    bool PlayCard(FName CardName);

    // ── 턴 종료 ───────────────────────────────────────────────────────────────

    // 턴 종료 시 Hand 전체를 DiscardPile 로 이동 (DeckComponent::DiscardAll 위임)
    UFUNCTION(BlueprintCallable, Category = "Card|Hand")
    void DiscardHand();

    // ── 조회 ─────────────────────────────────────────────────────────────────

    UFUNCTION(BlueprintPure, Category = "Card|Hand")
    const TArray<FName>& GetHand() const { return Hand; }

    UFUNCTION(BlueprintPure, Category = "Card|Hand")
    int32 GetHandCount() const { return Hand.Num(); }

    UFUNCTION(BlueprintPure, Category = "Card|Hand")
    bool HasCardInHand(FName CardName) const { return Hand.Contains(CardName); }

private:
    // ── Hand 배열 (GC 안전) ───────────────────────────────────────────────────
    UPROPERTY()
    TArray<FName> Hand;

    // 파일 상태(DrawPile/DiscardPile)를 위임할 DeckComponent 참조
    UPROPERTY()
    TObjectPtr<UDeckComponent> DeckComponent;

    void BroadcastHandChanged();
};
