// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Unit/CombatTypes.h"
#include "GimmickComponent.generated.h"

class UGimmickData;
class UStatComponent;
class AUnit;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseEntered, const FGimmickPhase&, Phase);

//기믹이 외부 시스템에 요청하는 Delegate
//CombatManager가 실제처리
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGimmickDamageRequest, ETargetType, TargetType, int32, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGimmickAnnounce, const FText&, Text);

UCLASS( ClassGroup=(Unit), meta=(BlueprintSpawnableComponent) )
class SLAYTHECHAMPIONS_API UGimmickComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGimmickComponent();

	//데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick")
	UGimmickData* Data = nullptr;

	//각 페이즈가 이미 발동했는지 추적(bOneShot 처리용)
	UPROPERTY(BlueprintReadOnly, Category = "Gimmick")
	TArray<bool> Fired;

	UPROPERTY(BlueprintReadOnly, Category = "Gimmick")
	int32 TurnCounter = 0;

	// CombatManager가 호출
	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	void OnTurnStart();

	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	void OnTurnEnd();
		


	// ── Intent 보강 API (NPCBrainComponent가 PlanNextAction 마지막에 호출) ──
/**
 * 다음 턴에 발동할 기믹 정보를 Intent에 미리 추가한다.
 * 패턴이 결정한 1차 Intent는 그대로 두고, GimmickWarning 필드만 채운다.
 * 서브클래스에서 override.
 */
	virtual void AugmentIntent(FIntent& InOutIntent) const {}


	/**
	 * 다음 턴(TurnCounter + 1)에 기믹 페이즈가 발동할 예정인지 미리보기.
	 * AugmentIntent 구현 보조용. 서브클래스에서 override.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gimmick")
	virtual bool WillTriggerNextTurn() const { return false; }




	// 외부 구독용 Delegate
	UPROPERTY(BlueprintAssignable, Category = "Gimmick")
	FOnPhaseEntered OnPhaseEntered;

	UPROPERTY(BlueprintAssignable, Category = "Gimmick")
	FOnGimmickDamageRequest OnGimmickDamageRequest;

	UPROPERTY(BlueprintAssignable, Category = "Gimmick")
	FOnGimmickAnnounce OnGimmickAnnounce;


protected:
	virtual void BeginPlay() override;

	// ── 서브클래스용 virtual hooks ──
	// OnTurnStart/End 내부에서 호출됨
	virtual void OnGimmickTurnStart() {}
	virtual void OnGimmickTurnEnd() {}




	//StatComponent.OnHPChanged에 자동바인딩
	UFUNCTION()
	virtual void HandleHPChanged(int32 OldHP, int32 NewHP) {}
	
	//Unit.OnUnitDied에 바인딩
	UFUNCTION()
	virtual void HandleOwnerDied(AUnit* Unit) {}


private:
	void CheckTriggers();
	bool EvaluateTrigger(const FGimmickPhase& Phase) const;
	void EnterPhase(int32 Index);
};
