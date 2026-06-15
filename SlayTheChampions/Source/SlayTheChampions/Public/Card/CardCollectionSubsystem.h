#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Card/CardDataTypes.h"
#include "CardCollectionSubsystem.generated.h"

/**
 * FCardCollectionFilter
 *
 * 도감 필터 조건 구조체.
 * 직업 / 희귀도 / 카드 타입 세 가지 조건을 조합해 필터링.
 * 각 배열이 비어있으면 해당 조건은 무시 (전체 허용).
 */
USTRUCT(BlueprintType)
struct FCardCollectionFilter
{
    GENERATED_BODY()

    // 직업 필터. 비어있으면 전체 직업 표시.
    UPROPERTY(BlueprintReadWrite, Category = "Filter")
    TArray<EJobClass> JobClasses;

    // 희귀도 필터. 비어있으면 전체 희귀도 표시.
    UPROPERTY(BlueprintReadWrite, Category = "Filter")
    TArray<ECardRarity> Rarities;

    // 카드 타입 필터. 비어있으면 전체 타입 표시.
    UPROPERTY(BlueprintReadWrite, Category = "Filter")
    TArray<ECardType> CardTypes;
};

/**
 * UCardCollectionSubsystem
 *
 * GameInstance Subsystem.
 * 도감 카드 목록 조회 및 필터링을 담당한다.
 *
 * [사용법 C++]
 *   UCardCollectionSubsystem* CS = GetGameInstance()->GetSubsystem<UCardCollectionSubsystem>();
 *   TArray<FCardDataRow> Cards = CS->GetAllCards();
 *
 * [사용법 Blueprint]
 *   GetGameInstance -> GetSubsystem(CardCollectionSubsystem)
 *   -> GetAllCards / GetFilteredCards
 */
UCLASS()
class SLAYTHECHAMPIONS_API UCardCollectionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    /**
     * 전체 카드 목록을 반환한다. (도감 초기 표시용)
     * DT_Cards 의 모든 행을 반환.
     */
    UFUNCTION(BlueprintPure, Category = "Collection")
    TArray<FCardDataRow> GetAllCards() const;

    /**
     * 필터 조건에 맞는 카드 목록을 반환한다.
     * Filter 의 각 배열이 비어있으면 해당 조건 무시 (전체 허용).
     *
     * @param Filter  필터 조건 (직업 / 희귀도 / 카드 타입)
     * @return 조건에 맞는 FCardDataRow 배열
     */
    UFUNCTION(BlueprintCallable, Category = "Collection")
    TArray<FCardDataRow> GetFilteredCards(const FCardCollectionFilter& Filter) const;
};
