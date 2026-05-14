#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CardDataTypes.h"
#include "CardSubsystem.generated.h"

/**
 * UCardSubsystem
 *
 * GameInstance Subsystem.
 * - DataTable 에서 카드 데이터를 로드·캐시한다.
 * - 직업별 덱 풀, 보상 카드 풀 조회를 제공한다.
 * - 직접 참조 없이 어디서든 GEngine->GetEngineSubsystem 류로 접근 가능.
 *
 * 사용 예 (C++):
 *   UCardSubsystem* CS = GetGameInstance()->GetSubsystem<UCardSubsystem>();
 *   const FCardDataRow* Row = CS->GetCard(FName("Warrior_Attack"));
 *
 * 사용 예 (Blueprint):
 *   GetGameInstance → GetSubsystem(CardSubsystem) → GetCard / GetCardsByClass
 */
UCLASS()
class SLAYTHECHAMPIONS_API UCardSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // ── Subsystem Lifecycle ───────────────────────────────────────────────
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // ── 데이터 테이블 설정 ────────────────────────────────────────────────
    /**
     * 런타임에 DataTable 을 교체하거나 다시 로드할 때 호출.
     * Initialize 에서 자동으로 호출되므로 보통 직접 호출할 필요 없음.
     */
    UFUNCTION(BlueprintCallable, Category = "Card|Data")
    void LoadCardDataTable(UDataTable* InTable);

    // ── 카드 조회 ─────────────────────────────────────────────────────────

    /**
     * RowName 으로 단일 카드 데이터를 가져온다. 없으면 nullptr.
     * (const 포인터 반환은 UFUNCTION 불가 → C++ 전용)
     */
    const FCardDataRow* GetCard(FName RowName) const;

    /** 직업에 해당하는 카드 RowName 목록 반환 (Any 직업 카드 포함). */
    UFUNCTION(BlueprintCallable, Category = "Card|Query")
    TArray<FName> GetCardNamesByClass(EJobClass JobClass) const;

    /**
     * 직업에 해당하는 FCardDataRow 포인터 목록 반환.
     * (const 포인터 배열은 UFUNCTION 불가 → C++ 전용)
     */
    TArray<const FCardDataRow*> GetCardsByClass(EJobClass JobClass) const;

    /**
     * 보상 카드 풀: 직업 + 희귀도 기반으로 후보 목록 반환.
     * (희귀도 필터링 없이 전체를 원하면 Rarity = Normal 이외를 Any 로 처리)
     */
    UFUNCTION(BlueprintCallable, Category = "Card|Reward")
    TArray<FName> GetRewardPool(EJobClass JobClass, ECardRarity MinRarity) const;

    /** 전체 카드 RowName 목록 */
    UFUNCTION(BlueprintCallable, Category = "Card|Query")
    TArray<FName> GetAllCardNames() const;

private:
    // ── 내부 상태 ─────────────────────────────────────────────────────────
    /** 에디터에서 할당: Content/Data/DT_Cards */
    UPROPERTY()
    TObjectPtr<UDataTable> CardDataTable;

    /** Rarity 수치화 헬퍼 (보상 풀 필터링용) */
    static int32 RarityToInt(ECardRarity Rarity);
};