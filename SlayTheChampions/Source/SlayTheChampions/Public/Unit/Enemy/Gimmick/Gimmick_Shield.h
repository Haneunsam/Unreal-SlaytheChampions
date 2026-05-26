// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit/Enemy/GimmickComponent.h"
#include "Gimmick_Shield.generated.h"

/**
 * 일정 스택만큼 피해를 흡수하는 방패기믹
 * 스택이 있는 동안 모든 피해를 1로 제한, 히트당 1스택 소모
 * 연타공격 을 요구하는 기믹
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldStackChanged, int32, OldStacks, int32, NewStacks);


UCLASS(ClassGroup = (Unit), meta = (BlueprintSpawnableComponent))
class SLAYTHECHAMPIONS_API UGimmick_Shield : public UGimmickComponent
{
	GENERATED_BODY()
	
public:
	//초기 스택 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Shield")
	int32 MaxStacks = 10;

	//현재 스택 수
	UPROPERTY(BlueprintReadOnly, Category = "Gimmick|Shield")
	int32 CurrentStacks = 0;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gimmick|Shield")
	bool HasShield() const { return CurrentStacks > 0; }

	// 공격자가 TakeDamage 호출 전에 이 함수로 데미지를 보정
   // 스택 있으면 1 반환 + 스택 소모, 없으면 원래 데미지 그대로 반환
	UFUNCTION(BlueprintCallable, Category = "Gimmick|Shield")
	int32 ProcessIncomingDamage(int32 RawDamage);

	virtual bool WillTriggerNextTurn() const override;

	// UI가 스택 변화를 구독
	UPROPERTY(BlueprintAssignable, Category = "Gimmick|Shield")
	FOnShieldStackChanged OnShieldStackChanged;

protected:
	virtual void BeginPlay() override;
};
