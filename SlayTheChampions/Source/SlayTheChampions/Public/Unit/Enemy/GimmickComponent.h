// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Unit/CombatTypes.h"
#include "Card/CardDataTypes.h"
#include "GimmickComponent.generated.h"

class UGimmickData;
class UStatComponent;
class AUnit;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseEntered, const FGimmickPhase&, Phase);

//����� �ܺ� �ý��ۿ� ��û�ϴ� Delegate
//CombatManager�� ����ó��
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGimmickDamageRequest, ETargetType, TargetType, int32, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGimmickAnnounce, const FText&, Text);

UCLASS( ClassGroup=(Unit), meta=(BlueprintSpawnableComponent) )
class SLAYTHECHAMPIONS_API UGimmickComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGimmickComponent();

	//������
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick")
	UGimmickData* Data = nullptr;

	//�� ����� �̹� �ߵ��ߴ��� ����(bOneShot ó����)
	UPROPERTY(BlueprintReadOnly, Category = "Gimmick")
	TArray<bool> Fired;

	UPROPERTY(BlueprintReadOnly, Category = "Gimmick")
	int32 TurnCounter = 0;

	// CombatManager�� ȣ��
	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	void OnTurnStart();
		
	UFUNCTION(BlueprintCallable, Category = "Gimmick")
	void OnTurnEnd();
	
	// �ܺ� ������ Delegate
	UPROPERTY(BlueprintAssignable, Category = "Gimmick")
	FOnPhaseEntered OnPhaseEntered;

	UPROPERTY(BlueprintAssignable, Category = "Gimmick")
	FOnGimmickDamageRequest OnGimmickDamageRequest;

	UPROPERTY(BlueprintAssignable, Category = "Gimmick")
	FOnGimmickAnnounce OnGimmickAnnounce;

protected:
	virtual void BeginPlay() override;

	// ���� ����Ŭ������ virtual hooks ����
	// OnTurnStart/End ���ο��� ȣ���
	virtual void OnGimmickTurnStart() {}
	virtual void OnGimmickTurnEnd() {}

	// StatComponent.OnHPChanged�� �ڵ� ���ε���
	UFUNCTION()
	virtual void HandleHPChanged(int32 OldHP, int32 NewHP) {}

	// Unit.OnUnitDied�� �ڵ� ���ε���
	UFUNCTION()
	virtual void HandleOwnerDied(AUnit* Unit) {}

private:
	void CheckTriggers();
	bool EvaluateTrigger(const FGimmickPhase& Phase) const;
	void EnterPhase(int32 Index);
};
