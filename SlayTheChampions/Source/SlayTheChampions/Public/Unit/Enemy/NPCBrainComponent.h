// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Unit/CombatTypes.h"
#include "NPCBrainComponent.generated.h"

class UEnemyPatternData;
class AUnit;

// ActionQueue와의 연결점, CombatManager가 이것을 사용
// 송출 타입을 FEnemyAction → FIntent로 변경.
// 이유: FEnemyAction에는 해결된 Target(AUnit*)이 없고 TargetType만 있다.
//       FIntent는 Target까지 해결되어 있고 EffectType/Value/Duration도 담겨 있어
//       CombatManager가 추가 가공 없이 한 턴을 그대로 실행할 수 있다.

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionEmitted, const FIntent&, Intent);

UCLASS( ClassGroup=(Unit), meta=(BlueprintSpawnableComponent) )
class SLAYTHECHAMPIONS_API UNPCBrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNPCBrainComponent();

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Brain")
	UEnemyPatternData* Pattern = nullptr;

	//Sequential모드에서 다음 행동 인덱스 추적
	UPROPERTY(BlueprintReadOnly,Category = "Brain")
	int32 SequenceIndex = 0;

	//결정된 행동 EmitActionEvent에서 사용
	UPROPERTY(BlueprintReadOnly, Category = "Brain")
	FEnemyAction PendingAction;	


	//PlanNextAction이 완성한 Intent. EmitActionEvent가 이것을 송출한다.
	UPROPERTY(BlueprintReadOnly, Category = "Brain")
	FIntent PendingIntent;


	//턴 시작시 CombatMananger 가 호출 ->행동결정 + IntentComponent갱신
	UFUNCTION(BlueprintCallable, Category = "Brain")
	void PlanNextAction(const TArray<AUnit*>& Allies, const TArray<AUnit*>& Enemies);


	//적 턴이 되면 CombatManager가 호출 ->ActionQueue로 송출
	UFUNCTION(BlueprintCallable, Category = "Brain")
	void EmitActionEvent();
	
	//기믹 컴포넌트에서 페이즈 전환시 호출
	// 추후 구현
	// GimmickComponent에서 페이즈 전환 시 호출
	/*
	UFUNCTION(BlueprintCallable, Category = "Brain")
	void SwapPattern(UEnemyPatternData* NewPattern);
	*/

	UPROPERTY(BlueprintAssignable, Category = "Brain")
	FOnActionEmitted OnActionEmitted;

private:
	FEnemyAction PickNext();
	AUnit* PickTarget(ETargetType TargetType,
		const TArray<AUnit*>& Allies,
		const TArray<AUnit*>& Enemies) const;

		
};
