// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Unit/CombatTypes.h"
#include "EnemyPatternData.generated.h"

/**
 * 에셋을 수정하면 다른적이 되게끔. 코드 수정 불필요
 */
UCLASS(BlueprintType)
class SLAYTHECHAMPIONS_API UEnemyPatternData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pattern")
	EPatternMode Mode = EPatternMode::Sequential;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pattern")
	TArray<FEnemyAction> Actions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pattern")
	bool bLoop = true;
};
