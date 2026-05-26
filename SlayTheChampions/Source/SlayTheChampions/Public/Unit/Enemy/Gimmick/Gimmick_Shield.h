// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit/Enemy/GimmickComponent.h"
#include "Gimmick_Shield.generated.h"

/**
 * 쉴드 스택만큼 데미지를 흡수하는 기믹 클래스
 * 쉴드가 있는 동안 모든 데미지를 1로 감소, 스택은 1씩 소모
 * 데이터는 에디터 내 설정이 필요한 경우
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldStackChanged, int32, OldStacks, int32, NewStacks);


UCLASS(ClassGroup = (Unit), meta = (BlueprintSpawnableComponent))
class SLAYTHECHAMPIONS_API UGimmick_Shield : public UGimmickComponent
{
	GENERATED_BODY()

public:
	// 초기 쉴드 스택
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Shield")
	int32 MaxStacks = 10;

	// 현재 쉴드 스택
	UPROPERTY(BlueprintReadOnly, Category = "Gimmick|Shield")
	int32 CurrentStacks = 0;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gimmick|Shield")
	bool HasShield() const { return CurrentStacks > 0; }

	// 공격자가 TakeDamage 호출 이전 이 함수를 먼저 통과시켜 사용
	// 쉴드 있으면 1 반환 + 스택 소모, 쉴드 없으면 원본 데미지 그대로 반환
	UFUNCTION(BlueprintCallable, Category = "Gimmick|Shield")
	int32 ProcessIncomingDamage(int32 RawDamage);

	// UI를 위한 스택 변화 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Gimmick|Shield")
	FOnShieldStackChanged OnShieldStackChanged;

protected:
	virtual void BeginPlay() override;
};
