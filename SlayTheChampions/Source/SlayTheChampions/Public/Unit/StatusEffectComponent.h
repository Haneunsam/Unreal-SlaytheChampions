// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Unit/StatusEffect.h"
#include "StatusEffectComponent.generated.h"

class AUnit;

UENUM(BlueprintType)
enum class EEffectType : uint8
{
	Block UMETA(DisplayName = "Block"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectApplied, UStatusEffect*, Effect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectRemoved, TSubclassOf<UStatusEffect>, EffectClass);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEffectValueChanged, EEffectType, Type, int32, OldValue, int32, NewValue);

UCLASS( ClassGroup=(Unit), meta=(BlueprintSpawnableComponent) )
class SLAYTHECHAMPIONS_API UStatusEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatusEffectComponent();

	//UstatEffect�ּҰ��� ������ �迭 ����
	UPROPERTY(BlueprintReadOnly, Category = "StatusEffect")
	TArray<UStatusEffect*> Active;

	//�Լ� ȿ�� ����, ����, ã�� �Լ�
	UFUNCTION(BlueprintCallable, Category = "StatusEffect")
	UStatusEffect* ApplyEffect(TSubclassOf<UStatusEffect> EffectClass, int32 Stacks, int32 Duration);

	UFUNCTION(BlueprintCallable, Category = "StatusEffect")
	void RemoveEffect(TSubclassOf<UStatusEffect> EffectClass);

	UFUNCTION(BlueprintCallable, Category = "StatusEffect")
	UStatusEffect* FindEffect(TSubclassOf<UStatusEffect> EffectClass) const;


	UPROPERTY(BlueprintAssignable) FOnEffectApplied OnEffectApplied;
	UPROPERTY(BlueprintAssignable) FOnEffectRemoved OnEffectRemoved;

	// 수치형 효과 (Block 등) 저장소
	UPROPERTY(BlueprintReadOnly, Category = "StatusEffect")
	TMap<EEffectType, int32> EffectValues;

	UFUNCTION(BlueprintCallable, Category = "StatusEffect")
	void SetEffectValue(EEffectType Type, int32 NewValue);

	UFUNCTION(BlueprintPure, Category = "StatusEffect")
	int32 GetEffectValue(EEffectType Type) const;

	UPROPERTY(BlueprintAssignable, Category = "StatusEffect")
	FOnEffectValueChanged OnEffectValueChanged;
};
