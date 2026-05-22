// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CombatTypes.generated.h"

class AUnit;

//진영 구분
UENUM(BlueprintType)
enum class ETeam : uint8
{
    Ally    UMETA(DisplayName = "Ally"),
    Enemy   UMETA(DisplayName = "Enemy"),
    Neutral UMETA(DisplayName = "Neutral")
};


// ──────────────────────────────────────────────
//  적 행동의 "의도(Intent)" 종류.
//  주의: 이 enum은 디자이너가 에디터에서 직접 고르는 값이 아니다.
//        FEnemyAction::ResolveIntentKind()가 ActionType + EffectType으로
//        자동 도출하는 "결과값"이다. UI 표시 / CombatManager 분기에만 사용.
// ──────────────────────────────────────────────
UENUM(BlueprintType)
enum class EIntentKind : uint8
{
    NoAttack = 0 UMETA(DisplayName = "NoAttack"),
    Attack UMETA(DisplayName = "Attack"),
    Defend UMETA(DisplayName = "Defend"),
    Buff    UMETA(DisplayName = "Buff"),
    Debuff  UMETA(DisplayName = "Debuff"),
    Unknown UMETA(DisplayName = "Unknown")
};

// ──────────────────────────────────────────────
//  상태 효과 종류 (단일 enum)
//  - 0          : None (효과 없음)
//  - 1          : Shield (보호막 — 별도 취급)
//  - 100~199    : Buff
//  - 200~299    : Debuff
//  값 범위로 IsBuff/IsDebuff를 분기한다.
// ──────────────────────────────────────────────
UENUM(BlueprintType)
enum class EEffectType : uint8
{
    None = 0   UMETA(DisplayName = "None"),
    Shield = 1 UMETA(DisplayName = "보호막"),
    // ── Buff (100~199) ─────────────────────
    Buff_AttackUp = 100 UMETA(DisplayName = "공격력 증가"),
    Buff_DefenseUp = 101 UMETA(DisplayName = "방어력 증가"),
    Buff_Regen = 102 UMETA(DisplayName = "재생"),

    // ── Debuff (200~299) ───────────────────
    Debuff_Weak = 200 UMETA(DisplayName = "약화"),
    Debuff_Vulnerable = 201 UMETA(DisplayName = "취약"),
    Debuff_Burn = 202 UMETA(DisplayName = "작열"),
    Debuff_Frail = 203 UMETA(DisplayName = "허약")
};

// ──────────────────────────────────────────────
//  EEffectType 헬퍼
//  범위 기반: enum 항목 추가 시 IsBuff/IsDebuff 수정 불필요
// ──────────────────────────────────────────────
namespace EffectTypeHelpers
{
    FORCEINLINE bool IsBuff(EEffectType Type)
    {
        const uint8 V = static_cast<uint8>(Type);
        return V >= 100 && V < 200;
    }

    FORCEINLINE bool IsDebuff(EEffectType Type)
    {
        const uint8 V = static_cast<uint8>(Type);
        return V >= 200 && V < 300;
    }

    FORCEINLINE EIntentKind ToIntentKind(EEffectType Type)
    {
        if (IsBuff(Type))   return EIntentKind::Buff;
        if (IsDebuff(Type)) return EIntentKind::Debuff;
        return EIntentKind::Unknown;
    }
}


// ──────────────────────────────────────────────
//  타겟 종류.
//  FIntent / FEnemyAction 보다 먼저 선언되어야 한다.
// ──────────────────────────────────────────────
UENUM(BlueprintType)
enum class ETargetType : uint8
{
    SingleEnemy UMETA(DisplayName = "Single Enemy"),
    ALlEnemies  UMETA(DisplayName = "All Enemies"),
    Self        UMETA(DisplayName = "Self"),
    RamdomAlly  UMETA(DisplayName = "Random Ally"),
    AllAlies    UMETA(DisplayName = "All Alies")
};

// ──────────────────────────────────────────────
//  적 행동 카테고리 (디자이너 전용 선택지).
//  에디터 드롭다운에는 이 3개만 노출된다.
//  버프/디버프는 여기서 고르지 않고, EffectType 필드로 부여한다.
// ──────────────────────────────────────────────
UENUM(BlueprintType)
enum class EActionType : uint8
{
    Attack   UMETA(DisplayName = "공격"),
    Defend   UMETA(DisplayName = "보호막"),
    NoAttack UMETA(DisplayName = "무행동")
};



//// ──────────────────────────────────────────────
////  런타임 상태 효과 인스턴스 (USTRUCT)
////  UStatusEffectComponent.Active 배열에 보관.
////  UObject가 아닌 POD라서 메모리/GC 부담 없음.
//// ──────────────────────────────────────────────
USTRUCT(BlueprintType)
struct SLAYTHECHAMPIONS_API FStatusEffects
{
    GENERATED_BODY()

    /** 효과 자신의 정체성 (UI 아이콘/dispatch 키) */
    UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Effect")
    EEffectType EffectType = EEffectType::None;

    /** 현재 값 (스택/강도). 의미는 EffectType마다 다름. */
    UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Effect")
    int32 Value = 0;

    /** 턴마다 Value에 더해지는 값. 디버프는 보통 -1. */
    UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Effect")
    int32 DeltaPerTurn = 0;

    /** 남은 적용 횟수. -1 = 무한, 0 = 즉시 제거. 제거 판정은 Count만 사용. */
    UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Effect")
    int32 Count = -1;

    /** 자기 턴 시작 시 무조건 제거. (Block 같은 자기 강화 패턴용) */
    UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Effect")
    bool bResetOnTurnStart = false;

    /** Value 클램프 최솟값. 제거 판정에는 사용하지 않음. */
    UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Effect")
    int32 FloorValue = -2147483648; // INT32_MIN
};





// ──────────────────────────────────────────────
//  적 의도(Intent).
//  UI 위젯이 이 구조체만 읽어서 머리 위에 아이콘을 표시한다.
//  CombatManager도 이 구조체 하나만 받아서 적의 한 턴을 실행한다.
//  즉 UI와 전투 실행 양쪽의 "공용 계약서".
// ──────────────────────────────────────────────
USTRUCT(BlueprintType)
struct SLAYTHECHAMPIONS_API FIntent
{
    GENERATED_BODY()

    // ── 패턴이 결정하는 1차 의도 (모든 적 공통) ─────────────
    UPROPERTY(BlueprintReadOnly) EIntentKind Kind = EIntentKind::Unknown;
    UPROPERTY(BlueprintReadOnly) int32       Value = 0;  // 예상 데미지 or 블록량
    UPROPERTY(BlueprintReadOnly) int32       Hits = 1;

    /** 타겟 범위. nullptr Target만 봐서는 광역/사망구분이 안 되므로 반드시 필요. */
    UPROPERTY(BlueprintReadOnly) ETargetType TargetType = ETargetType::SingleEnemy;

    /** 해결된 단일 대상. 광역(ALlEnemies/AllAlies)일 때는 무시되고 null일 수 있음. */
    UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<AUnit> Target;

    UPROPERTY(BlueprintReadOnly) FText       DisplayText;

    // ── 이 행동에 딸린 상태효과 (None이면 효과 없음) ─────────
    /** CombatManager가 StatusEffectComponent::AApplyEffect(Type,Value,Duration)로 그대로 전달. */
    UPROPERTY(BlueprintReadOnly) EEffectType EffectType = EEffectType::None;
    UPROPERTY(BlueprintReadOnly) int32       EffectValue = 0;
    UPROPERTY(BlueprintReadOnly) int32       EffectDuration = 0;

    // ── 보스 기믹이 보강하는 추가 정보 (보스 전용, 선택) ───
    /** 다음 턴에 기믹이 발동할 예정인지 여부. UI가 경고 아이콘 표시용으로 사용. */
    UPROPERTY(BlueprintReadOnly) bool        bHasGimmickWarning = false;
    /** 기믹 경고 텍스트 (예: "다음 턴 분노 발동", "방패 활성화 중") */
    UPROPERTY(BlueprintReadOnly) FText       GimmickWarningText;
};


UENUM(BlueprintType)
enum class EPatternMode : uint8
{
    Sequential UMETA(DisplayName = "Sequential"), //순서대로 반복
    Weighted UMETA(DisplayName = "Weighted")//가중치랜덤
};



// ──────────────────────────────────────────────
//  적 행동 한 개의 정의. EnemyPatternData.Actions 배열에 저장됨.
//  ActionType : 디자이너가 고르는 행동 카테고리 (공격/보호막/무행동)
//  EffectType : 이 행동에 함께 부여할 상태효과 (None이면 효과 없음)
//  IntentKind는 직접 고르지 않고 ResolveIntentKind()가 자동 도출한다.
// ──────────────────────────────────────────────
USTRUCT(BlueprintType)
struct SLAYTHECHAMPIONS_API FEnemyAction
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly) EActionType  ActionType = EActionType::Attack;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) int32        Value = 0;   // 데미지 or 블록량
    UPROPERTY(EditAnywhere, BlueprintReadOnly) int32        Hits = 1;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) ETargetType  TargetType = ETargetType::SingleEnemy;

    // ── 이 행동에 딸린 상태효과 ──────────────────────────
    /** 부여할 효과 종류. None이면 효과 없음. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly) EEffectType  EffectType = EEffectType::None;
    /** 효과 강도/스택. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly) int32        EffectValue = 0;
    /** 효과 지속 턴. -1 = 무한, 0 = 즉시 제거. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly) int32        EffectDuration = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly) float        Weight = 1.f;   // Weighted 모드에서만 사용
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText        DisplayName;

    /**
     * ActionType + EffectType으로 최종 EIntentKind를 도출한다.
     * - Attack  → Attack
     * - Defend  → Defend
     * - NoAttack → 효과가 있으면 Buff/Debuff, 없으면 NoAttack
     * 구현은 CombatTypes.cpp 참고.
     */
    EIntentKind ResolveIntentKind() const;
};

UENUM(BlueprintType)
enum class EGimmickTrigger : uint8
{
    HPThresholdBelow UMETA(DisplayName = "HP Below Threshold"), // HP가 X% 이하
    TurnCountReached UMETA(DisplayName = "Turn Count Reached"), // N턴 경과
    OnDamaged        UMETA(DisplayName = "On Damaged")          // 피격 시
};

USTRUCT(BlueprintType)
struct SLAYTHECHAMPIONS_API FGimmickPhase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName           PhaseName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) EGimmickTrigger Trigger = EGimmickTrigger::HPThresholdBelow;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float           TriggerValue = 0.5f; // HP면 0~1 비율, Turn이면 턴 수
    //UPROPERTY(EditAnywhere, BlueprintReadOnly) UEnemyPatternData* SwapToPattern = nullptr;
    //UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<TSubclassOf<UStatusEffect>> ApplyOnEnter;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText           AnnounceText;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool            bOneShot = true; // true면 한 번만 발동
};

