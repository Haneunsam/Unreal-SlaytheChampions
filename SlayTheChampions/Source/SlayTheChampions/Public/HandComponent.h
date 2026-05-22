#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CardDataTypes.h"
#include "HandComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHandChanged, const TArray<FName>&, CurrentHand);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardDrawn, FName, DrawnCard);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardPlayed, FName, PlayedCard);

/**
 * UHandComponent
 *
 * 파티원 1명의 덱/손패/버리기 더미를 관리하는 컴포넌트.
 * 특정 Actor 에 종속되지 않도록 ActorComponent 로 설계.
 * Enemy 에도 붙일 수 있게 범용 설계.
 *
 * 책임:
 *  - 덱 초기화 및 관리
 *  - 드로우 (DrawPile 에서 뽑아 DiscardPile 자동 순환)
 *  - 카드 사용 -> 손패에서 제거 -> DiscardPile 이동
 *  - 턴 종료 시 손패 전체 버리기
 *
 * 게임 규칙(코스트 소모 등)은 이 컴포넌트 밖에서 처리한다.
 */
UCLASS(ClassGroup = "Card", meta = (BlueprintSpawnableComponent))
class SLAYTHECHAMPIONS_API UHandComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHandComponent();

    // 상태 이벤트 ----------------------------------------------------------------
    UPROPERTY(BlueprintAssignable, Category = "Card|Hand")
    FOnHandChanged OnHandChanged;

    UPROPERTY(BlueprintAssignable, Category = "Card|Hand")
    FOnCardDrawn OnCardDrawn;

    UPROPERTY(BlueprintAssignable, Category = "Card|Hand")
    FOnCardPlayed OnCardPlayed;

    // 설정 변수 ------------------------------------------------------------------
    // 턴 시작 기본 드로우 수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card|Hand",
        meta = (ClampMin = "1"))
    int32 DefaultDrawCount = 5;

    // 덱 초기화 ------------------------------------------------------------------

    // 덱을 초기화하고 드로우 파일 준비
    UFUNCTION(BlueprintCallable, Category = "Card|Hand")
    void InitializeDeck(const TArray<FName>& InDeckNames);

    // 카드 드로우 ----------------------------------------------------------------

    // DefaultDrawCount 만큼 드로우
    UFUNCTION(BlueprintCallable, Category = "Card|Hand")
    void DrawStartOfTurn();

    // N장 드로우. DrawPile 부족 시 DiscardPile 순환
    UFUNCTION(BlueprintCallable, Category = "Card|Hand")
    TArray<FName> DrawCards(int32 Count);

    // 카드 사용 ------------------------------------------------------------------

    // 손패에서 카드를 사용 (DiscardPile 이동). 손패에 없으면 false 반환
    UFUNCTION(BlueprintCallable, Category = "Card|Hand")
    bool PlayCard(FName CardName);

    // 손패 전체 버리기 -----------------------------------------------------------

    // 손패 전체를 DiscardPile 로 이동
    UFUNCTION(BlueprintCallable, Category = "Card|Hand")
    void DiscardHand();

    // 상태 조회 ------------------------------------------------------------------

    // 손패 반환
    UFUNCTION(BlueprintPure, Category = "Card|Hand")
    const TArray<FName>& GetHand() const { return Hand; }

    // DrawPile 전체 반환 (SaveGame 저장용)
    UFUNCTION(BlueprintPure, Category = "Card|Hand")
    const TArray<FName>& GetDrawPile() const { return DrawPile; }

    // DiscardPile 전체 반환 (SaveGame 저장용)
    UFUNCTION(BlueprintPure, Category = "Card|Hand")
    const TArray<FName>& GetDiscardPile() const { return DiscardPile; }

    UFUNCTION(BlueprintPure, Category = "Card|Hand")
    int32 GetDrawPileCount() const { return DrawPile.Num(); }

    UFUNCTION(BlueprintPure, Category = "Card|Hand")
    int32 GetDiscardPileCount() const { return DiscardPile.Num(); }

    UFUNCTION(BlueprintPure, Category = "Card|Hand")
    int32 GetHandCount() const { return Hand.Num(); }

    // 손패에 해당 카드가 있는지 확인
    UFUNCTION(BlueprintPure, Category = "Card|Hand")
    bool HasCardInHand(FName CardName) const { return Hand.Contains(CardName); }

private:
    // 내부 상태 관리 -------------------------------------------------------------
    TArray<FName> DrawPile;
    TArray<FName> Hand;
    TArray<FName> DiscardPile;

    // DiscardPile -> DrawPile 순환 + 셔플
    void RecycleDiscardIntoDraw();

    void BroadcastHandChanged();
};
