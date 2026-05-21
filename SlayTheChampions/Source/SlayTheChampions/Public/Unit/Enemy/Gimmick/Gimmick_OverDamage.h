// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit/Enemy/GimmickComponent.h"
#include "Gimmick_OverDamage.generated.h"

/**
 * 일정 데미지를 입히지 않으면 큰 데미지를 주는 기믹 (극딜요구)
 * 여기서 일정 데미지 넣는 걸 계속 해야하는 건지 아니면 한번 나오는건지 결정 필요
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

	// 구간 내 필요 데미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|OverDamage")
	int32 RequiredDamage = 30;

	// 현재 구간 누적 피해량
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|OverDamage")
	int32 AccumulatedDamage = 0;

	// 실패시 입히는 데미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|DPSCheck")
	int32 RageDamage = 50;

	// 분노 연출용 텍스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|DPSCheck")
	FText RageAnnounce;

protected:
	virtual void OnGimmickTurnStart() override;
	virtual void HandleHPChanged(int32 OldHP, int32 NewHP) override;
};
