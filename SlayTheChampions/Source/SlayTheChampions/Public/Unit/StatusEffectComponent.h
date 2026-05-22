// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Unit/StatusEffect.h"
#include "StatusEffectComponent.generated.h"

class AUnit;

/**
 * EEffectType
 * 수치형 효과의 종류. TMap 키로 사용.
 * None=0 으로 고정. Shield(보호막)=1.
 * Buff 100~199 / Debuff 200~299 로 확장 예정.
 */
UENUM(BlueprintType)
enum class EEffectType : uint8
{
	None   = 0  UMETA(DisplayName = "None"),
	Shield = 1  UMETA(DisplayName = "보호막"),  // 데미지 흡수, 턴 시작 시 리셋
};

// 오브젝트형 효과 적용/제거 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectApplied, UStatusEffect*, Effect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectRemoved, TSubclassOf<UStatusEffect>, EffectClass);
// 수치형 효과 변경 이벤트 (UI 갱신용)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEffectValueChanged, EEffectType, Type, int32, OldValue, int32, NewValue);

/**
 * UStatusEffectComponent
 * 유닛에 부착되어 버프/디버프를 관리하는 컴포넌트.
 *
 * 두 가지 저장 방식을 병용:
 *  - Active (TArray<UStatusEffect*>) : 오브젝트형 효과 (독, 화상 등 매 턴 동작)
 *  - EffectValues (TMap<EEffectType, int32>) : 수치형 효과 (Shield 등 단순 수치)
 */
UCLASS( ClassGroup=(Unit), meta=(BlueprintSpawnableComponent) )
class SLAYTHECHAMPIONS_API UStatusEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatusEffectComponent();

	// ── 오브젝트형 효과 (Active 배열) ────────────────────────────────────

	// 현재 활성화된 오브젝트형 효과 목록
	UPROPERTY(BlueprintReadOnly, Category = "StatusEffect")
	TArray<UStatusEffect*> Active;

	// 오브젝트형 효과 적용 (같은 타입이면 스택 합산)
	UFUNCTION(BlueprintCallable, Category = "StatusEffect")
	UStatusEffect* ApplyEffect(TSubclassOf<UStatusEffect> EffectClass, int32 Stacks, int32 Duration);

	// 오브젝트형 효과 제거
	UFUNCTION(BlueprintCallable, Category = "StatusEffect")
	void RemoveEffect(TSubclassOf<UStatusEffect> EffectClass);

	// 오브젝트형 효과 탐색 (없으면 nullptr)
	UFUNCTION(BlueprintCallable, Category = "StatusEffect")
	UStatusEffect* FindEffect(TSubclassOf<UStatusEffect> EffectClass) const;

	UPROPERTY(BlueprintAssignable) FOnEffectApplied OnEffectApplied;
	UPROPERTY(BlueprintAssignable) FOnEffectRemoved OnEffectRemoved;

	// ── 수치형 효과 (EffectValues TMap) ──────────────────────────────────

	// EEffectType → 수치 저장소 (Shield 등)
	UPROPERTY(BlueprintReadOnly, Category = "StatusEffect")
	TMap<EEffectType, int32> EffectValues;

	// 수치 설정 및 OnEffectValueChanged 브로드캐스트
	UFUNCTION(BlueprintCallable, Category = "StatusEffect")
	void SetEffectValue(EEffectType Type, int32 NewValue);

	// 현재 수치 조회 (없으면 0 반환)
	UFUNCTION(BlueprintPure, Category = "StatusEffect")
	int32 GetEffectValue(EEffectType Type) const;

	// 수치 변경 이벤트 (UI 자동 갱신용)
	UPROPERTY(BlueprintAssignable, Category = "StatusEffect")
	FOnEffectValueChanged OnEffectValueChanged;
};
