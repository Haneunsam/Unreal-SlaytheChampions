#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Card/CardDataTypes.h"
#include "Card/HandComponent.h"   // FOnHandChanged, FOnCardPlayed 정의 위치
#include "CardUserComponent.generated.h"

class UDeckComponent;

/**
 * UCardUserComponent
 *
 * 카드 시스템을 사용하는 유닛(플레이어)에 붙이는 컴포넌트.
 * AUnit 기반 BP에 이 컴포넌트 하나만 Add Component 하면 된다.
 *
 * 역할:
 *  - BeginPlay 시 DeckComponent 를 Owner 에 동적 생성
 *  - Hand 배열 직접 보유 및 관리
 *  - 덱 초기화 (SaveGame → OverrideDeckNames → CardSubsystem 순)
 *  - CardManager 에 DeckComponent 등록
 *  - 턴 조작 함수 제공 (DrawStartOfTurn, PlayCard, DiscardHand)
 *  - 전투 종료 후 덱 SaveGame 저장
 *
 * 사용법:
 *  1. BP 에서 Add Component → CardUserComponent 하나만 추가
 *  2. 디테일 패널에서 PawnIndex, JobClass, OverrideDeckNames 설정
 */
UCLASS(ClassGroup = "Card", meta = (BlueprintSpawnableComponent))
class SLAYTHECHAMPIONS_API UCardUserComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCardUserComponent();

protected:
    virtual void BeginPlay() override;

public:
    // ── 이벤트 델리게이트 ────────────────────────────────────────────────────

    // Hand 변경 시 브로드캐스트 (UI 연동용)
    UPROPERTY(BlueprintAssignable, Category = "Card")
    FOnHandChanged OnHandChanged;

    // 카드 사용 시 브로드캐스트
    UPROPERTY(BlueprintAssignable, Category = "Card")
    FOnCardPlayed OnCardPlayed;

    // ── 설정 ─────────────────────────────────────────────────────────────────

    /**
     * 파티 내 인덱스.
     * SaveGame 에서 덱 불러올 때 사용.
     * 0 = Pawn1 (Warrior), 1 = Pawn2 (Mage)
     * 에디터에서 각 유닛에 맞게 설정 필요.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card")
    int32 PawnIndex = 0;

    // 이 파티원의 직업 (카드 조회 및 덱 초기화에 사용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card")
    EJobClass JobClass = EJobClass::Warrior;

    /**
     * 에디터에서 직접 지정할 덱 목록. (테스트용)
     * 비어있으면 SaveGame 또는 CardSubsystem 사용.
     * 실제 게임에서는 비워둘 것.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card")
    TArray<FName> OverrideDeckNames;

    // ── 덱 초기화 / 저장 ─────────────────────────────────────────────────────

    /**
     * 덱을 초기화한다.
     * BeginPlay 에서 자동 호출.
     * SaveGame → OverrideDeckNames → CardSubsystem 순 우선순위.
     */
    UFUNCTION(BlueprintCallable, Category = "Card")
    void InitializeDeck();

    /**
     * 전투 종료 후 현재 덱 상태를 SaveGame 에 저장한다.
     * DrawPile + Hand + DiscardPile 합산 저장 (ExhaustPile 제외).
     * 전투 종료 이벤트에서 호출할 것.
     */
    UFUNCTION(BlueprintCallable, Category = "Card")
    void SaveDeckToSaveGame();

    // ── 턴 조작 ──────────────────────────────────────────────────────────────

    // 턴 시작 시 카드 드로우 (기본 5장)
    UFUNCTION(BlueprintCallable, Category = "Card")
    void DrawStartOfTurn();

    // N장 드로우
    UFUNCTION(BlueprintCallable, Category = "Card")
    void DrawCards(int32 Count);

    /**
     * Hand 에서 카드를 사용한다.
     * bExhaust 카드면 ExhaustPile, 아니면 DiscardPile 로 이동.
     * @return 사용 성공 여부 (Hand 에 없으면 false)
     */
    UFUNCTION(BlueprintCallable, Category = "Card")
    bool PlayCard(FName CardName);

    // 턴 종료 시 Hand 전체를 DiscardPile 로 버린다.
    UFUNCTION(BlueprintCallable, Category = "Card")
    void DiscardHand();

    // Hand 에서만 카드를 제거한다 (DiscardPile 이동 없음).
    // 카드 큐 등록 시 손패 UI 즉시 갱신 용도.
    // @return 제거 성공 여부 (Hand 에 없으면 false)
    UFUNCTION(BlueprintCallable, Category = "Card")
    bool RemoveFromHand(FName CardName);

    // Hand 에 카드를 직접 추가하고 OnHandChanged 브로드캐스트.
    // 큐 취소 시 카드를 손패로 되돌리는 용도.
    UFUNCTION(BlueprintCallable, Category = "Card")
    void AddToHand(FName CardName);

    // 카드를 DiscardPile(또는 bExhaust면 ExhaustPile)로 이동한다.
    // Hand 는 건드리지 않음 — RemoveFromHand 이후 실행 완료 시 호출.
    UFUNCTION(BlueprintCallable, Category = "Card")
    void DiscardSpecificCard(FName CardName);

    // ── 조회 ─────────────────────────────────────────────────────────────────

    UFUNCTION(BlueprintPure, Category = "Card")
    const TArray<FName>& GetHand() const { return Hand; }

    UFUNCTION(BlueprintPure, Category = "Card")
    int32 GetHandCount() const { return Hand.Num(); }

    UFUNCTION(BlueprintPure, Category = "Card")
    int32 GetDrawPileCount() const;

    UFUNCTION(BlueprintPure, Category = "Card")
    int32 GetDiscardPileCount() const;

    UFUNCTION(BlueprintPure, Category = "Card")
    int32 GetExhaustPileCount() const;

    // DeckComponent 참조 반환 (CardManager 등 외부 접근용)
    UFUNCTION(BlueprintPure, Category = "Card")
    UDeckComponent* GetDeckComponent() const { return DeckComponent; }

private:
    // Hand 배열 (GC 안전)
    UPROPERTY()
    TArray<FName> Hand;

    // BeginPlay 에서 Owner 에 동적 생성
    UPROPERTY()
    TObjectPtr<UDeckComponent> DeckComponent;

    // 턴 시작 기본 드로우 수
    int32 DefaultDrawCount = 5;

    // Hand 변경 시 델리게이트 브로드캐스트
    void BroadcastHandChanged();

    // bExhaust 카드 여부 조회
    bool IsExhaustCard(FName CardName) const;
};
