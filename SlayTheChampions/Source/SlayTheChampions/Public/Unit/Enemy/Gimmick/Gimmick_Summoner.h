// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit/Enemy/GimmickComponent.h"
#include "Gimmick_Summoner.generated.h"

class AUnit;
/**
 * 하수인 소환해서 하수인이 있는동안 강력한 버프와 공격을한다. 
 */
 // CombatManager가 구독: 하수인 스폰 요청
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMinionSpawnRequested, int32, Count);

UCLASS(ClassGroup = (Unit), meta = (BlueprintSpawnableComponent))
class SLAYTHECHAMPIONS_API UGimmick_Summoner : public UGimmickComponent
{
	GENERATED_BODY()
	
public:
	// 소환할 하수인 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Summon")
	int32 MinionCount = 2;

	// 하수인이 살아 있는 동안 매 턴 회복량
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Gimmick|Summon")
	int32 HealAmount = 5;

	// 소환 쿨타임 (0 이면 전멸 후 재소환)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Summon")
	int32 SummonCooldown = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Minion")
	FText SummonAnnounce;

	// 하수인 존재 여부
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gimmick|Minion")
	bool HasAliveMinion() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gimmick|Minion")
	int32 GetAliveMinionCount() const;

	// CombatManager가 스폰 후 호출
	UFUNCTION(BlueprintCallable, Category = "Gimmick|Minion")
	void RegisterMinion(AUnit* Minion);

	UPROPERTY(BlueprintAssignable, Category = "Gimmick|Minion")
	FOnMinionSpawnRequested OnMinionSpawnRequested;

protected:
	virtual void BeginPlay() override;
	virtual void OnGimmickTurnStart() override;

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<AUnit>> SpawnedMinions;

	// 마지막 소환 이후 경과 턴
	int32 TurnsSinceLastSummon = 0;
	bool bHasSummoned = false;
};
