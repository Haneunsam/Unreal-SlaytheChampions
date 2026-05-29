#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Card/CardDataTypes.h"
#include "Unit/StatusEffectComponent.h"
#include "EffectStruct.generated.h"

UENUM(BlueprintType)
/* 효과를 실제로 적용하는 타이밍 */
enum class EEffectApplyTiming : uint8
{
	OnUse = 0 UMETA(DisplayName = "On Use"),
	OnAcquire = 1 UMETA(DisplayName = "On Acquire"),
	OnMap = 2 UMETA(DisplayName = "On Map"),
	InShop = 3 UMETA(DisplayName = "In Shop"),
	OnBattleStart = 4 UMETA(DisplayName = "On Battle Start"),
	OnBattleEnd = 5 UMETA(DisplayName = "On Battle End"),
	DuringBattle = 6 UMETA(DisplayName = "During Battle"),
	OnDeath = 7 UMETA(DisplayName = "On Death"),
	None = 99 UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
/* 효과 발동 조건 */
enum class EEffectTriggerCondition : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	TurnCountReached = 1 UMETA(DisplayName = "Turn Count Reached"),
	StackCountReached = 2 UMETA(DisplayName = "Stack Count Reached"),
};

UENUM(BlueprintType)
/* 효과 발동 방식 */
enum class EEffectTriggerUsageType : uint8
{
	OneShot = 0 UMETA(DisplayName = "One Shot"),
	Repeat = 1 UMETA(DisplayName = "Repeat"),
};

USTRUCT(BlueprintType)
/* 카드, 유물, 포션이 공통으로 사용할 효과 CSV 행 */
struct FSourceEffectRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SourceID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Order = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEffectType EffectType = EEffectType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Value = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETargetType TargetScope = ETargetType::Self;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEffectApplyTiming ApplyTiming = EEffectApplyTiming::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEffectTriggerCondition TriggerCondition = EEffectTriggerCondition::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TriggerValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEffectTriggerUsageType TriggerUsageType = EEffectTriggerUsageType::OneShot;
};

USTRUCT(BlueprintType)
/* 런타임에서 사용하는 공통 효과 데이터 */
struct FSourceEffectData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Order = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEffectType EffectType = EEffectType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Value = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETargetType TargetScope = ETargetType::Self;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEffectApplyTiming ApplyTiming = EEffectApplyTiming::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEffectTriggerCondition TriggerCondition = EEffectTriggerCondition::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TriggerValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEffectTriggerUsageType TriggerUsageType = EEffectTriggerUsageType::OneShot;
};
