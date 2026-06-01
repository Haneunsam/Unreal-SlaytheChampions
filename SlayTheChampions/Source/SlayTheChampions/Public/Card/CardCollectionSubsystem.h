#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Card/CardDataTypes.h"
#include "CardCollectionSubsystem.generated.h"

/**
 * ECardLockState
 *
 * 도감에서 카드 한 장의 잠금 상태.
 * Blueprint 에서 이 값에 따라 UI 표시 방식을 결정한다.
 */
UENUM(BlueprintType)
enum class ECardLockState : uint8
{
    // 획득 완료 - 카드 정상 표시
    Unlocked        UMETA(DisplayName = "Unlocked"),

    // 미획득 일반 카드 - 실루엣(어둡게) 표시, 카드 이름은 보임
    LockedNormal    UMETA(DisplayName = "LockedNormal"),

    // 미획득 업적 전용 카드 - 완전 검정, 이름/정보 완전 숨김
    LockedAchievement UMETA(DisplayName = "LockedAchievement"),
};

/**
 * FCardCollectionEntry
 *
 * 도감 한 장의 표시 데이터.
 * GetFilteredCards() 가 반환하는 단위.
 */
USTRUCT(BlueprintType)
struct FCardCollectionEntry
{
    GENERATED_BODY()

    // 카드 ID (DT_Cards RowName)
    UPROPERTY(BlueprintReadOnly, Category = "Collection")
    FName CardID;

    // 도감 잠금 상태 (Unlocked / LockedNormal / LockedAchievement)
    UPROPERTY(BlueprintReadOnly, Category = "Collection")
    ECardLockState LockState = ECardLockState::LockedNormal;
};

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

    /**
     * 직업 필터.
     * 비어있으면 전체 직업 표시.
     * 값이 있으면 해당 직업 + Any 카드만 표시.
     */
    UPROPERTY(BlueprintReadWrite, Category = "Filter")
    TArray<EJobClass> JobClasses;

    /**
     * 희귀도 필터.
     * 비어있으면 전체 희귀도 표시.
     */
    UPROPERTY(BlueprintReadWrite, Category = "Filter")
    TArray<ECardRarity> Rarities;

    /**
     * 카드 타입 필터.
     * 비어있으면 전체 타입 표시.
     */
    UPROPERTY(BlueprintReadWrite, Category = "Filter")
    TArray<ECardType> CardTypes;
};

/**
 * UCardCollectionSubsystem
 *
 * GameInstance Subsystem.
 * 도감 기능을 담당한다.
 *
 * [카드 잠금 상태 구분]
 * - Unlocked          : 획득 완료, 정상 표시
 * - LockedNormal      : 미획득 일반 카드, 실루엣 표시
 * - LockedAchievement : 미획득 업적 전용 카드, 완전 검정 표시
 *
 * [사용법 C++]
 *   UCardCollectionSubsystem* CS = GetGameInstance()->GetSubsystem<UCardCollectionSubsystem>();
 *   CS->UnlockCard(TEXT("Warrior_Attack_01"));
 *
 * [사용법 Blueprint]
 *   GetGameInstance -> GetSubsystem(CardCollectionSubsystem)
 *   -> UnlockCard / IsUnlocked / GetFilteredCards
 */
UCLASS()
class SLAYTHECHAMPIONS_API UCardCollectionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // ── Subsystem 라이프사이클 ──────────────────────────────────────────────────

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // ── 획득 등록 ─────────────────────────────────────────────────────────────

    /**
     * 카드를 획득 목록에 등록하고 SaveGame 에 저장한다.
     * 보상/상점 구매 시 CardSaveGame::AddCard() 와 함께 호출.
     * 업적 전용 카드(bAchievementOnly=true) 도 이 함수로 등록.
     *
     * @param CardID  DT_Cards RowName
     */
    UFUNCTION(BlueprintCallable, Category = "Collection")
    void UnlockCard(FName CardID);

    // ── 획득 조회 ─────────────────────────────────────────────────────────────

    /**
     * 카드가 획득된 상태인지 반환한다.
     *
     * @param CardID  DT_Cards RowName
     * @return 획득했으면 true
     */
    UFUNCTION(BlueprintPure, Category = "Collection")
    bool IsUnlocked(FName CardID) const;

    /**
     * 카드 한 장의 잠금 상태를 반환한다.
     * Blueprint UI 에서 표시 방식 결정에 사용.
     *
     * @param CardID  DT_Cards RowName
     * @return ECardLockState (Unlocked / LockedNormal / LockedAchievement)
     */
    UFUNCTION(BlueprintPure, Category = "Collection")
    ECardLockState GetLockState(FName CardID) const;

    /**
     * 획득한 카드 ID 목록 전체를 반환한다.
     */
    UFUNCTION(BlueprintPure, Category = "Collection")
    TArray<FName> GetUnlockedCards() const;

    // ── 도감 필터 조회 ────────────────────────────────────────────────────────

    /**
     * 도감 표시용 카드 목록을 반환한다.
     * - 전체 카드(DT_Cards) 기준으로 필터 조건 적용
     * - 미획득 카드도 포함 (LockState 로 표시 방식 결정)
     * - Filter 의 각 배열이 비어있으면 해당 조건 무시 (전체 허용)
     *
     * @param Filter  필터 조건 (직업 / 희귀도 / 카드 타입)
     * @return FCardCollectionEntry 배열 (CardID + LockState)
     */
    UFUNCTION(BlueprintCallable, Category = "Collection")
    TArray<FCardCollectionEntry> GetFilteredCards(const FCardCollectionFilter& Filter) const;

    /**
     * 필터 없이 전체 카드 목록을 반환한다. (도감 초기 표시용)
     */
    UFUNCTION(BlueprintPure, Category = "Collection")
    TArray<FCardCollectionEntry> GetAllCards() const;

private:
    // 런타임 획득 목록 캐시 (SaveGame 로드 후 메모리에 유지, 디스크 접근 최소화)
    UPROPERTY()
    TArray<FName> CachedUnlockedCards;

    /**
     * 카드 ID 와 획득 여부를 받아 FCardCollectionEntry 를 생성하는 헬퍼.
     *
     * @param CardID      DT_Cards RowName
     * @param bUnlocked   획득 여부
     * @param bAchievement 업적 전용 카드 여부 (DT_Cards.bAchievementOnly)
     */
    FCardCollectionEntry MakeEntry(FName CardID, bool bUnlocked, bool bAchievement) const;
};
