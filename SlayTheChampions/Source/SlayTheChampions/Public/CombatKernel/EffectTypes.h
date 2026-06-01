#pragma once

#include "CoreMinimal.h"
#include "EffectTypes.generated.h"

/**
 * EEffectType
 * 전투 중 적용되는 모든 효과의 종류.
 * StatusEffectComponent의 TMap 키, RelicStruct의 BuffType 등에서 공통으로 사용.
 *
 * 범위 구분:
 *   0~99   : 즉발/수치형 효과 (Shield, Heal, Damage 등)
 *   100~199: 버프 (공격력 증가, 재생 등)
 *   200~299: 디버프 (약화, 화상 등)
 *
 * uint8 최대값(255) 초과 불가 — 299 이상 추가 시 uint16으로 변경 필요.
 */
UENUM(BlueprintType)
enum class EEffectType : uint8
{
	None                   = 0   UMETA(DisplayName = "None"),

	// ── 즉발 / 수치형 효과 (0~99) ────────────────────────────────────────
	Shield                 = 1   UMETA(DisplayName = "Shield"),           // 데미지 흡수, 턴 시작 시 리셋
	Heal                   = 2   UMETA(DisplayName = "Heal"),
	MaxHPUp                = 3   UMETA(DisplayName = "Max HP Up"),
	DrawCard               = 4   UMETA(DisplayName = "Draw Card"),
	GainEnergy             = 5   UMETA(DisplayName = "Gain Energy"),
	Summon                 = 6   UMETA(DisplayName = "Summon"),
	ChannelLightning       = 7   UMETA(DisplayName = "Channel Lightning"),
	UpgradeAttackCard      = 8   UMETA(DisplayName = "Upgrade Attack Card"),
	Thorns                 = 9   UMETA(DisplayName = "Thorns"),           // 공격받을 때 반사 데미지
	PoisonBonus            = 10  UMETA(DisplayName = "Poison Bonus"),
	Damage                 = 11  UMETA(DisplayName = "Damage"),
	VulnerableDefenseBonus = 12  UMETA(DisplayName = "Vulnerable Defense Bonus"),
	VulnerableAttackBonus  = 13  UMETA(DisplayName = "Vulnerable Attack Bonus"),

	// ── 버프 (100~199) ───────────────────────────────────────────────────
	Buff_AttackUp          = 100 UMETA(DisplayName = "Attack Up"),        // 공격력 증가
	Buff_DefenseUp         = 101 UMETA(DisplayName = "Defense Up"),       // 방어력 증가
	Buff_Regen             = 102 UMETA(DisplayName = "Regen"),            // 매 턴 HP 회복

	// ── 디버프 (200~299) ─────────────────────────────────────────────────
	Debuff_Weak            = 200 UMETA(DisplayName = "Weak"),             // 공격력 감소
	Debuff_Vulnerable      = 201 UMETA(DisplayName = "Vulnerable"),       // 받는 데미지 증가
	Debuff_Burn            = 202 UMETA(DisplayName = "Burn"),             // 매 턴 데미지
	Debuff_Frail           = 203 UMETA(DisplayName = "Frail"),            // 방어력 감소
};

/**
 * EEffectTargetType
 * FCardEffect 개별 효과의 대상 지정.
 * UseCardDefault = 카드의 기본 TargetType을 그대로 사용.
 * 나머지는 이 효과에만 독립적으로 적용되는 대상.
 */
UENUM(BlueprintType)
enum class EEffectTargetType : uint8
{
	UseCardDefault  UMETA(DisplayName = "Use Card Default"), // 카드의 TargetType 그대로 사용
	SingleEnemy     UMETA(DisplayName = "Single Enemy"),
	AllEnemies      UMETA(DisplayName = "All Enemies"),
	Self            UMETA(DisplayName = "Self"),
	SingleAlly      UMETA(DisplayName = "Single Ally"),
	AllAllies       UMETA(DisplayName = "All Allies"),
};

/**
 * FCardEffect
 * 카드·유물 효과 항목 1개.
 * FCardDataRow::Effects 배열의 원소로 사용.
 * 한 카드에 여러 효과(데미지 + 디버프, 버프 2종 등)를 지정할 수 있다.
 */
USTRUCT(BlueprintType)
struct FCardEffect
{
	GENERATED_BODY()

	// 효과 종류 (에디터에서 드롭다운으로 선택)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	EEffectType EffectType = EEffectType::None;

	// 효과 수치 (스택 수, 데미지량, 회복량 등)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect",
		meta = (ClampMin = "0"))
	int32 Value = 0;

	// 이 효과의 대상 — None이면 카드의 기본 TargetType 사용
	// 예: 데미지는 SingleEnemy, 버프는 Self를 같은 카드에 공존시킬 때 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	EEffectTargetType TargetType = EEffectTargetType::UseCardDefault;
};
