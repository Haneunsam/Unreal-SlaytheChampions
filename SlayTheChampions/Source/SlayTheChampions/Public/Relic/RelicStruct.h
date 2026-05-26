// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CardDataTypes.h"
#include "RelicStruct.generated.h"

UENUM(BlueprintType)
enum class EEffectType_test : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Shield = 1 UMETA(DisplayName = "Shield"),
	Heal = 2 UMETA(DisplayName = "Heal"),
	MaxHPUp = 3 UMETA(DisplayName = "Max HP Up"),
	DrawCard = 4 UMETA(DisplayName = "Draw Card"),
	GainEnergy = 5 UMETA(DisplayName = "Gain Energy"),
	Summon = 6 UMETA(DisplayName = "Summon"),
	ChannelLightning = 7 UMETA(DisplayName = "Channel Lightning"),
	UpgradeAttackCard = 8 UMETA(DisplayName = "Upgrade Attack Card"),
	Thorns = 9 UMETA(DisplayName = "Thorns"),
	PoisonBonus = 10 UMETA(DisplayName = "Poison Bonus"),
	Damage = 11 UMETA(DisplayName = "Damage"),
	VulnerableDefenseBonus = 12 UMETA(DisplayName = "Vulnerable Defense Bonus"),
	VulnerableAttackBonus = 13 UMETA(DisplayName = "Vulnerable Attack Bonus"),

	Buff_AttackUp = 100 UMETA(DisplayName = "Attack Up"),
	Buff_DefenseUp = 101 UMETA(DisplayName = "Defense Up"),
	Buff_Regen = 102 UMETA(DisplayName = "Regen"),

	Debuff_Weak = 200 UMETA(DisplayName = "Weak"),
	Debuff_Vulnerable = 201 UMETA(DisplayName = "Vulnerable"),
	Debuff_Burn = 202 UMETA(DisplayName = "Burn"),
	Debuff_Frail = 203 UMETA(DisplayName = "Frail")
};

UENUM(BlueprintType)
/*유물 적용 타이밍*/
enum class EEffectiveDate : uint8
{
	OnAcquire = 0 UMETA(DisplayName = "On Acquire"),
	OnMap = 1 UMETA(DisplayName = "On Map"),
	InShop = 2 UMETA(DisplayName = "In Shop"),
	OnBattleStart = 3 UMETA(DisplayName = "On Battle Start"),
	OnBattleEnd = 4 UMETA(DisplayName = "On Battle End"),
	DuringBattle = 5 UMETA(DisplayName = "During Battle"),
	None = 99 UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
/*유물 획득 경로*/
enum class ERelicSourceType : uint8
{
	Default = 0 UMETA(DisplayName = "Default"),
	Shop = 1 UMETA(DisplayName = "Shop"),
	Event = 2 UMETA(DisplayName = "Event"),
	Common = 3 UMETA(DisplayName = "Common"),
	None = 99 UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
/*유물 희귀도*/
enum class ERelicRarity : uint8
{
	Normal = 0 UMETA(DisplayName = "Normal"),
	Rare = 1 UMETA(DisplayName = "Rare"),
	Legendary = 2 UMETA(DisplayName = "Legendary"),
	None = 99 UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
/*유물 발동 조건*/
enum class ERelicTriggerCondition : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	TurnCountReached = 1 UMETA(DisplayName = "Turn Count Reached"),
	StackCountReached = 2 UMETA(DisplayName = "Stack Count Reached"),
};

UENUM(BlueprintType)
/*유물 발동 방식*/
enum class ERelicTriggerUsageType : uint8
{
	OneShot = 0 UMETA(DisplayName = "One Shot"),
	Repeat = 1 UMETA(DisplayName = "Repeat"),
};

USTRUCT(BlueprintType)
/*유물 기본 정보 구조체*/
struct FRelicDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RelicID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText RelicName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ERelicRarity Rarity = ERelicRarity::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ERelicSourceType RelicSourceType = ERelicSourceType::Common;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETargetType TargetScope = ETargetType::Self;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEffectiveDate EffectiveDate = EEffectiveDate::None;
};

USTRUCT(BlueprintType)
/*유물 효과 정보 구조체*/
struct FRelicEffectRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RelicID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Order = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEffectType_test BuffType = EEffectType_test::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Value = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEffectiveDate EffectiveDate = EEffectiveDate::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ERelicTriggerCondition TriggerCondition = ERelicTriggerCondition::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TriggerValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ERelicTriggerUsageType TriggerUsageType = ERelicTriggerUsageType::OneShot;
};

USTRUCT(BlueprintType)
struct FRelicEffectData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Order = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEffectType_test BuffType = EEffectType_test::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Value = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEffectiveDate EffectiveDate = EEffectiveDate::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ERelicTriggerCondition TriggerCondition = ERelicTriggerCondition::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TriggerValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ERelicTriggerUsageType TriggerUsageType = ERelicTriggerUsageType::OneShot;
};

USTRUCT(BlueprintType)
struct FRelicRuntimeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RelicID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText RelicName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ERelicRarity Rarity = ERelicRarity::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ERelicSourceType RelicSourceType = ERelicSourceType::Common;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETargetType TargetScope = ETargetType::Self;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEffectiveDate EffectiveDate = EEffectiveDate::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FRelicEffectData> Effects;
};
