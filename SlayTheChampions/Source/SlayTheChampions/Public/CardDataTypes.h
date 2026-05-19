#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CardDataTypes.generated.h"

// ──────────────────────────────────────────────
//  Enum : 카드 희귀도
// ──────────────────────────────────────────────
UENUM(BlueprintType)
enum class ECardRarity : uint8
{
    Normal    UMETA(DisplayName = "Normal"),
    Rare      UMETA(DisplayName = "Rare"),
    Legendary UMETA(DisplayName = "Legendary"),
    Status    UMETA(DisplayName = "Status"),
};

// ──────────────────────────────────────────────
//  Enum : 카드 타입
// ──────────────────────────────────────────────
UENUM(BlueprintType)
enum class ECardType : uint8
{
    Attack  UMETA(DisplayName = "Attack"),
    Defense UMETA(DisplayName = "Defense"),
    Skill   UMETA(DisplayName = "Skill"),
    Buff    UMETA(DisplayName = "Buff"),
    AOE     UMETA(DisplayName = "AOE"),
    Status  UMETA(DisplayName = "Status"),
};

// ──────────────────────────────────────────────
//  Enum : 직업 분류
// ──────────────────────────────────────────────
UENUM(BlueprintType)
enum class EJobClass : uint8
{
    Any     UMETA(DisplayName = "Any"),
    Warrior UMETA(DisplayName = "Warrior"),
    Mage    UMETA(DisplayName = "Mage"),
    Healer  UMETA(DisplayName = "Healer"),
};

// ──────────────────────────────────────────────
//  Enum : 대상 범위
// ──────────────────────────────────────────────
UENUM(BlueprintType)
enum class ETargetType : uint8
{
    SingleEnemy UMETA(DisplayName = "SingleEnemy"),
    AllEnemies  UMETA(DisplayName = "AllEnemies"),
    Self        UMETA(DisplayName = "Self"),
    SingleAlly  UMETA(DisplayName = "SingleAlly"),
    AllAllies   UMETA(DisplayName = "AllAllies"),
    Single_Team UMETA(DisplayName = "Single_Team"),  // Healer 단일 아군 방어 전용
};

// ──────────────────────────────────────────────
//  DataTable Row Struct
//  Import path: DT_Cards.csv → DataTable (FCardDataRow)
// ──────────────────────────────────────────────
USTRUCT(BlueprintType)
struct FCardDataRow : public FTableRowBase
{
    GENERATED_BODY()

    // ── 식별 ──────────────────────────────────
    /** 카드 ID (DataTable RowName 과 동일하게 유지) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Identity")
    FName Name;

    /** 인게임 표시 이름 (한글) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Identity")
    FText CardName;

    /** 카드 설명 텍스트 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Identity")
    FText Description;

    // ── 분류 ──────────────────────────────────
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Category")
    ECardRarity Rarity = ECardRarity::Normal;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Category")
    ECardType CardType = ECardType::Attack;

    /** 이 카드를 사용할 수 있는 직업. Any = 공용 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Category")
    EJobClass RequiredClass = EJobClass::Any;

    // ── 코스트 ────────────────────────────────
    /** 파티 공용 코스트 소모량 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Cost",
        meta = (ClampMin = "0", ClampMax = "10"))
    int32 Cost = 1;

    // ── 효과 수치 ─────────────────────────────
    /** 기본 피해량 (0 = 공격 없음) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Effect",
        meta = (ClampMin = "0"))
    int32 Damage = 0;

    /** 부여할 방어도 (0 = 방어 없음) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Effect",
        meta = (ClampMin = "0"))
    int32 Block = 0;

    /** 카드 사용 시 드로우할 장수 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Effect",
        meta = (ClampMin = "0"))
    int32 DrawCount = 0;

    /**
     * 카드 사용 횟수 (대부분 1).
     * DoubleAttack 처럼 동일 효과를 N회 반복할 때 사용.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Effect",
        meta = (ClampMin = "1"))
    int32 UsingCount = 1;

    /** 회복량 (힐러 전용, 0 = 회복 없음) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Effect",
        meta = (ClampMin = "0"))
    int32 HealAmount = 0;

    // ── 특수 효과 ─────────────────────────────
    /**
     * 특수 효과 태그 (GameplayTag 문자열 또는 커스텀 키).
     * 비어있으면 특수 효과 없음.
     * 예: "Buff.DefenseUp", "Debuff.Weak", "Field.Fire"
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Effect")
    FName EffectTag;

    /** EffectTag 에 대응하는 스택/지속 수치 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Effect",
        meta = (ClampMin = "0"))
    int32 EffectValue = 0;

    // ── 대상 ──────────────────────────────────
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Target")
    ETargetType TargetType = ETargetType::SingleEnemy;

    // ── 밸런스 ────────────────────────────────
    /** 기획 밸류 점수 (카드 밸류 기준표 기반) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Balance",
        meta = (ClampMin = "0"))
    int32 ScoreValue = 0;

    // ── 메모 ──────────────────────────────────
    /** 기획 노트 (런타임 미사용) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Meta")
    FString Notes;
};