// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Unit/CombatTypes.h"
#include "GimmickData.generated.h"

/**
 * 보스몬스터에게만 달아주는 Gimmick데이터
 */
UCLASS(BlueprintType)
class SLAYTHECHAMPIONS_API UGimmickData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick")
	TArray<FGimmickPhase> Phases;
};
