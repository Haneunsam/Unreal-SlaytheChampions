// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Unit/CombatTypes.h"
#include "CardDataTypes.h"
#include "GimmickComponent.generated.h"

class UGimmickData;
class UStatComponent;
class AUnit;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseEntered, const FGimmickPhase&, Phase);

// 기믹이 외부 시스템에 요청하는 Delegate
// CombatManager에서 처리
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGimmickDamageRequest, ETargetType, TargetType, int32, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGimmickAnnounce, const FText&, Text);

UCLASS( ClassGroup=(Unit), meta=(BlueprintSpawnableComponent) )
class SLAYTHECHAMPIONS_API UGimmickComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGimmickComponent();

	// 기믹 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick")
	UGimmickData* Data = nullptr;

	// 각 페이즈가 이미 발동했는지 여부(bOneShot 처리용)
	UPROPERTY(BlueprintReadOnly, Category = "Gimmick")
	TArray<bool> Fired;

	UPROPERTY(BlueprintReadOnly, Category = "Gimmick")
	int32 TurnCounter = 0;

	// CombatManager에서 호출
	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	void OnTurnStart();

	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	void OnTurnEnd();

	// 외부 시스템 Delegate
	UPROPERTY(BlueprintAssignable, Category = "Gimmick")
	FOnPhaseEntered OnPhaseEntered;

	UPROPERTY(BlueprintAssignable, Category = "Gimmick")
	FOnGimmickDamageRequest OnGimmickDamageRequest;

	UPROPERTY(BlueprintAssignable, Category = "Gimmick")
	FOnGimmickAnnounce OnGimmickAnnounce;

protected:
	virtual void BeginPlay() override;

	// 파생 서브클래스용 virtual hooks 정의
	// OnTurnStart/End 내부에서 호출됨
	virtual void OnGimmickTurnStart() {}
	virtual void OnGimmickTurnEnd() {}

	// StatComponent.OnHPChanged에 자동 바인딩됨
	UFUNCTION()
	virtual void HandleHPChanged(int32 OldHP, int32 NewHP) {}

	// Unit.OnUnitDied에 자동 바인딩됨
	UFUNCTION()
	virtual void HandleOwnerDied(AUnit* Unit) {}

private:
	void CheckTriggers();
	bool EvaluateTrigger(const FGimmickPhase& Phase) const;
	void EnterPhase(int32 Index);
};
