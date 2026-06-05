// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Card/CardDataTypes.h"
#include "Unit/Enemy/EnemyPatternData.h"
#include "Unit/Enemy/GimmickComponent.h"
#include "Unit/StatusEffect.h"
#include "Unit/CombatTypes.h"
#include "EnemyDefinitionData.generated.h"

class UGimmickData;

USTRUCT(BlueprintType)
struct FStartingEffect
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere) TSubclassOf<UStatusEffect> EffectClass;
	UPROPERTY(EditAnywhere) int32 Stacks = 1;
	UPROPERTY(EditAnywhere) int32 Duration = -1;
};
/**
 * 
 */
UCLASS(BlueprintType)
class SLAYTHECHAMPIONS_API UEnemyDefinitionData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Identity") FName EnemyID;
	UPROPERTY(EditAnywhere, Category = "Identity") FText DisplayName;
	UPROPERTY(EditAnywhere, Category = "Identity") EEnemyTier Tier = EEnemyTier::Normal;

	//Ω∫≈»
	UPROPERTY(EditAnywhere, Category = "Stat") int32 MaxHP = 100;
	UPROPERTY(EditAnywhere, Category = "Stat") int32 BaseDefence = 0;

	UPROPERTY(EditAnywhere, Category = "Behavior") TObjectPtr<UEnemyPatternData> Pattern;
	UPROPERTY(EditAnywhere, Category = "Behavior") TArray<FStartingEffect> StartingEffects;

	UPROPERTY(EditAnywhere, Category = "Gimmick") TSubclassOf<UGimmickComponent> GimmickClass;
	UPROPERTY(EditAnywhere, Category = "Gimmick") TObjectPtr<UGimmickData> GimmickData;
};
