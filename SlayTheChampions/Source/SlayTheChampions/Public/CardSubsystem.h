#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CardDataTypes.h"
#include "CardSubsystem.generated.h"

/**
 * UCardSubsystem
 *
 * GameInstance Subsystem.
 * - DataTable 기반 카드 데이터를 로드·캐싱한다.
 * - 직업별 덱 풀, 보상 카드 풀 조회를 담당한다.
 * - 어디서든 GetGameInstance()->GetSubsystem<UCardSubsystem>() 형식으로 접근.
 *
 * 사용 예 (C++):
 *   UCardSubsystem* CS = GetGameInstance()->GetSubsystem<UCardSubsystem>();
 *   const FCardDataRow* Row = CS->GetCard(FName("Warrior_Attack"));
 *
 * 사용 예 (Blueprint):
 *   GetGameInstance 의 GetSubsystem(CardSubsystem) -> GetCard / GetCardsByClass
 */
UCLASS()
class SLAYTHECHAMPIONS_API UCardSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // 서브시스템 Lifecycle
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // 데이터 테이블 로드
    /**
     * 런타임에 DataTable 을 교체하거나 다시 로드할 때 호출.
     * Initialize 에서 자동으로 호출되므로 일반 사용 시 호출할 필요 없음.
     */
    UFUNCTION(BlueprintCallable, Category = "Card|Data")
    void LoadCardDataTable(UDataTable* InTable);

    // 카드 조회

    /**
     * RowName 으로 해당 카드 데이터를 가져온다. 없으면 nullptr.
     * (const 포인터 반환은 UFUNCTION 불가 -> C++ 전용)
     */
    const FCardDataRow* GetCard(FName RowName) const;

    /** 직업에 해당하는 카드 RowName 목록 반환 (Any 포함 카드 포함). */
    UFUNCTION(BlueprintCallable, Category = "Card|Query")
    TArray<FName> GetCardNamesByClass(EJobClass JobClass) const;

    /**
     * 직업에 해당하는 FCardDataRow 포인터 목록 반환.
     * (const 포인터 배열은 UFUNCTION 불가 -> C++ 전용)
     */
    TArray<const FCardDataRow*> GetCardsByClass(EJobClass JobClass) const;

    /**
     * 보상 카드 풀: 직업 + 최소 희귀도 기준으로 선별 목록 반환.
     */
    UFUNCTION(BlueprintCallable, Category = "Card|Reward")
    TArray<FName> GetRewardPool(EJobClass JobClass, ECardRarity MinRarity) const;

    /** 전체 카드 RowName 목록 반환 */
    UFUNCTION(BlueprintCallable, Category = "Card|Query")
    TArray<FName> GetAllCardNames() const;

private:
    // 내부 상태 관리
    /** 에셋에서 할당: Content/01_Card/01_Test_DT/DT_Cards */
    UPROPERTY()
    TObjectPtr<UDataTable> CardDataTable;

    /** Rarity 수치화 헬퍼 (보상 풀 필터링용) */
    static int32 RarityToInt(ECardRarity Rarity);
};
