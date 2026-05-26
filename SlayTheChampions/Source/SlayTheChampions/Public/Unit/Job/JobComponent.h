// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "JobComponent.generated.h"

/*
* 직업별 고유 자원이나 특수한 카드가 사용 될 때 처리로직을 처리
*/

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLAYTHECHAMPIONS_API UJobComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UJobComponent();

	// 카드 시스템이 카드 사용 후 호출 -> 직업별 자원 갱신

	UFUNCTION(BlueprintNativeEvent, Category = "Job")
	void OnCardPlayed(FGameplayTag CardTag, int32 CardValue);
	virtual void OnCardPlayed_Implementation(FGameplayTag CardTag, int32 CardValue) {}


	// 카드 데미지 계산 시점 호출 -> 직업 처리로직 반영
	UFUNCTION(BlueprintNativeEvent, Category = "Job")
	float ModifyCardDamage(FGameplayTag CardTag, float BaseDamage);
	virtual float ModifyCardDamage_Implementation(FGameplayTag CardTag, float BaseDamage) { return BaseDamage; }


	UFUNCTION(BlueprintNativeEvent, Category = "Job")
	void OnTurnStart();
	virtual void OnTurnStart_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Job")
	void OnTurnEnd();
	virtual void OnTurnEnd_Implementation() {}

};
