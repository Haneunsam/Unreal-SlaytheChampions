// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit/Enemy/GimmickComponent.h"
#include "Gimmick_OverDamage.generated.h"

/**
 * 일정 턴마다 누적된 데미지가 기준에 못 미칠 경우 큰 데미지를 주는 기믹 (DPS 체크)
 * 여기서 말하는 데미지를 주는 게 목표가 아니라 한번 발동하고서도 계속 반복이 필요
 *
 */
UCLASS(ClassGroup = (Unit), meta = (BlueprintSpawnableComponent))
class SLAYTHECHAMPIONS_API UGimmick_OverDamage : public UGimmickComponent
{
	GENERATED_BODY()

public:
	// N 턴마다 체크
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|OverDamage")
	int32 CheckInterval = 3;

	// 해당 턴 내 필요 데미지량
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|OverDamage")
	int32 RequiredDamage = 30;

	// 해당 구간 누적 받은 데미지량
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|OverDamage")
	int32 AccumulatedDamage = 0;

	// 분노 시 가하는 데미지량
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|DPSCheck")
	int32 RageDamage = 50;

	// 분노 발동 시 텍스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|DPSCheck")
	FText RageAnnounce;

protected:
	virtual void OnGimmickTurnEnd() override;
	virtual void HandleHPChanged(int32 OldHP, int32 NewHP) override;
};
