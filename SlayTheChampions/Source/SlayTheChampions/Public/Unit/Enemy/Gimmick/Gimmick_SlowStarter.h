// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit/Enemy/GimmickComponent.h"
#include "Gimmick_SlowStarter.generated.h"

/**
 * 처음 등장시 행동을 하지 않고, 시간이 지나면 엄청 강해진다.
 */
// 각성시 ComBatmanager가 구독하여 사용

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossAwakened);

UCLASS(ClassGroup = (Unit), meta = (BlueprintSpawnableComponent))
class SLAYTHECHAMPIONS_API UGimmick_SlowStarter : public UGimmickComponent
{
	GENERATED_BODY()
	
public:
	// 수면 지속 턴 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Sleep")
	int32 SleepDuration = 3;

	// 각성 시 적용할 버프 강도 (StatusEffect 스택 등)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Sleep")
	int32 AwakenBuffValue = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Sleep")
	FText AwakenAnnounce;

	UPROPERTY(BlueprintReadOnly, Category = "Gimmick|Sleep")
	bool bSleeping = true;

	// NPCBrainComponent가 호출하여 행동 여부 판단
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gimmick|Sleep")
	bool IsSleeping() const { return bSleeping; }

	virtual bool SuppressesAction() const override;

	// 피격 시 조기 각성 옵션 (데이터로 ON/OFF)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Sleep")
	bool bWakeOnDamage = false;

	virtual bool WillTriggerNextTurn() const override;

	UPROPERTY(BlueprintAssignable, Category = "Gimmick|Sleep")
	FOnBossAwakened OnAwakened;

protected:
	virtual void OnGimmickTurnStart() override;
	virtual void HandleHPChanged(int32 OldHP, int32 NewHP) override;

private:
	void Awaken();

};
