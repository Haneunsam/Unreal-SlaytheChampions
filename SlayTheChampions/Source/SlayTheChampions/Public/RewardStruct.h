// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RewardStruct.generated.h"

UENUM(BlueprintType)
/* 골드 획득량 */
enum class EGoldAmount : uint8
{
	None = 0,
	MIN_GOLD = 10,
	NOMAL_GOLD = 30,
	ELITE_GOLD = 50,
	BOSS_GOLD = 100,
};

UENUM(BlueprintType)
/* 유물 확률 */
enum class ERelicChance : uint8
{
	None = 0,
	NOMAL_CARD = 60,
	RARE_CARD = 30,
	LEGENDARY_CARD = 10,
};

UENUM(BlueprintType)
/* 카드 확률 */
enum class ECardChance : uint8
{
	None = 0,
	NOMAL = 60,
	RARE = 30,
	LEGENDARY = 10,
};

UENUM(BlueprintType)
/* 포션 확률 */
enum class EPotionChance : uint8
{
	None = 0,
	NOMAL = 80,
	RARE = 20,
};

USTRUCT(BlueprintType)
/* 보상 확률 */
struct FRewardChance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float NomalCardChance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RareCardChance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LegendaryCardChance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GoldChance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PotionChance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RelicChance = 0.f;

	FRewardChance() {}

	FRewardChance(float _nomal, float _Rare, float _Legend, float _Gold, float _Potion, float _Relic)
	{
		NomalCardChance = _nomal;
		RareCardChance = _Rare;
		LegendaryCardChance = _Legend;
		GoldChance = _Gold;
		PotionChance = _Potion;
		RelicChance = _Relic;
	}
};

/* Area별 보상 확률 */
USTRUCT(BlueprintType)
struct FNomalAreaChance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRewardChance Chance{ 100.f, 0.f, 0.f, 100.f, 30.f, 0.f };
};

USTRUCT(BlueprintType)
struct FEliteAreaChance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRewardChance Chance{ 100.f, 0.f, 0.f, 100.f, 42.5f, 100.f };
};

USTRUCT(BlueprintType)
struct FBossAreaChance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRewardChance Chance{ 0.f, 0.f, 100.f, 100.f, 50.f, 0.f };
};

USTRUCT(BlueprintType)
/* Area별 보상 생성 확률 */
struct FAreaRewardConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FNomalAreaChance nomal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FEliteAreaChance elite;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBossAreaChance boss;
};
