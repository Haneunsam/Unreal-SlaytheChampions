////// Fill out your copyright notice in the Description page of Project Settings.
////
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StatusEffect.generated.h"

class AUnit;
/**
 * 여기에서 상태이상 로직 설계
 */
UCLASS()
class SLAYTHECHAMPIONS_API UStatusEffect : public UObject
{
	GENERATED_BODY()
	
public:
	//스택
	UPROPERTY(BlueprintReadOnly, Category = "Effect")
	int32 Stacks = 0;

	// 지속시간 -1 infinite 0> decrease every turn 
	UPROPERTY(BlueprintReadOnly, Category = "Effect")
	int32 Duration = -1;

	//보유자
	UPROPERTY(BlueprintReadOnly, Category = "Effect")
	AUnit* Owner = nullptr;

	virtual void OnApplied() {}
	virtual void OnRemoved() {}
	virtual void OnTurnEnd(); //Duration감소 처리

	bool IsExpired() const { return Duration == 0 || Stacks <= 0; }
};
////
////
//////상태이상 구현부
//////1. 보호막버프
//////2. 공격력증가버프
//////3. 받는피해증가디버프
//////4. 주는피해감소디버프 
