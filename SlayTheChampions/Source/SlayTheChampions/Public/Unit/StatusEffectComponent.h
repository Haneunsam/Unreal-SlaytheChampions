// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Unit/StatusEffect.h"
#include "StatusEffectComponent.generated.h"

class AUnit;


// UI용
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectApplied, UStatusEffect*, Effect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectRemoved, TSubclassOf<UStatusEffect>, EffectClass);

UCLASS( ClassGroup=(Unit), meta=(BlueprintSpawnableComponent) )
class SLAYTHECHAMPIONS_API UStatusEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStatusEffectComponent();

	// UStatusEffect 주소값으로 관리하는 배열 목록
	UPROPERTY(BlueprintReadOnly, Category = "StatusEffect")
	TArray<UStatusEffect*> Active;

	// 효과 적용, 제거, 찾기 함수
	UFUNCTION(BlueprintCallable, Category = "StatusEffect")
	UStatusEffect* ApplyEffect(TSubclassOf<UStatusEffect> EffectClass, int32 Stacks, int32 Duration);

	UFUNCTION(BlueprintCallable, Category = "StatusEffect")
	void RemoveEffect(TSubclassOf<UStatusEffect> EffectClass);

	UFUNCTION(BlueprintCallable, Category = "StatusEffect")
	UStatusEffect* FindEffect(TSubclassOf<UStatusEffect> EffectClass) const;


	UPROPERTY(BlueprintAssignable) FOnEffectApplied OnEffectApplied;
	UPROPERTY(BlueprintAssignable) FOnEffectRemoved OnEffectRemoved;
};
