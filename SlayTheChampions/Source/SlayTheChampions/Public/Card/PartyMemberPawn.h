#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Card/CardDataTypes.h"
#include "PartyMemberPawn.generated.h"

class UHandComponent;
class UDeckComponent;

/**
 * APartyMemberPawn
 *
 * 파티원 1명을 나타내는 Pawn 베이스 클래스.
 * HandComponent(패 관리)와 DeckComponent(DrawPile/DiscardPile)를 보유한다.
 * BP_Pawn1, BP_Pawn2 의 부모 클래스로 사용.
 *
 * 덱 초기화 우선순위:
 * 1. SaveGame 파일에 저장된 덱 (전투 종료 후 저장된 덱)
 * 2. OverrideDeckNames (에디터/테스트용 직접 지정)
 * 3. CardSubsystem 에서 직업 기반 자동 조회
 */
UCLASS()
class SLAYTHECHAMPIONS_API APartyMemberPawn : public APawn
{
    GENERATED_BODY()

public:
    APartyMemberPawn();

protected:
    virtual void BeginPlay() override;

public:
    // 패(Hand) 관리 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Card")
    TObjectPtr<UHandComponent> HandComponent;

    // DrawPile / DiscardPile 관리 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Card")
    TObjectPtr<UDeckComponent> DeckComponent;

    /**
     * 파티 내 인덱스.
     * SaveGame 에서 덱 불러올 때 사용.
     * 0 = Pawn1 (Warrior), 1 = Pawn2 (Mage)
     * 에디터에서 각 Pawn 에 맞게 설정 필요.
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

    // ── 덱 관련 함수 ─────────────────────────────────────────────────────────

    /**
     * 덱을 초기화하고 HandComponent/DeckComponent 에 전달한다.
     * BeginPlay 에서 자동 호출.
     * SaveGame -> OverrideDeck -> CardSubsystem 순 우선순위.
     */
    UFUNCTION(BlueprintCallable, Category = "Card")
    void InitializeDeck();

    /**
     * 전투 종료 후 현재 덱 상태를 SaveGame 에 저장한다.
     * DrawPile + Hand + DiscardPile 을 합산해 저장 (A방식).
     * 전투 종료 이벤트에서 호출할 것.
     */
    UFUNCTION(BlueprintCallable, Category = "Card")
    void SaveDeckToSaveGame();

    // ── 턴/카드 조작 함수 ────────────────────────────────────────────────────

    // 턴 시작 시 카드 드로우 (기본 5장)
    UFUNCTION(BlueprintCallable, Category = "Card")
    void DrawStartOfTurn();

    /**
     * Hand 에서 카드를 사용한다.
     * @return 사용 성공 여부 (Hand 에 없으면 false)
     */
    UFUNCTION(BlueprintCallable, Category = "Card")
    bool PlayCard(FName CardName);

    // 턴 종료 시 Hand 전체를 DiscardPile 로 버린다.
    UFUNCTION(BlueprintCallable, Category = "Card")
    void DiscardHand();

    // ── 조회 함수 ────────────────────────────────────────────────────────────

    // 현재 Hand 카드 목록 반환
    UFUNCTION(BlueprintPure, Category = "Card")
    TArray<FName> GetHand() const;

    // DrawPile 남은 장 수
    UFUNCTION(BlueprintPure, Category = "Card")
    int32 GetDrawPileCount() const;

    // DiscardPile 장 수
    UFUNCTION(BlueprintPure, Category = "Card")
    int32 GetDiscardPileCount() const;
};
