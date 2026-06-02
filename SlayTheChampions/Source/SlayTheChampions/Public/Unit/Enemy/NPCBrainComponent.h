// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
///
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Unit/CombatTypes.h"
#include "Unit/PartyComponent.h" //PickTarget에서 GetRandomAliveMember등 사용
#include "NPCBrainComponent.generated.h"

class UEnemyPatternData;
class AUnit;

// ActionQueue와의 연결점, CombatManager가 이것을 사용
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionEmitted, const FEnemyAction&, Action);

/*
* 적 유닛의 행동을 결정하고 Intent를 내보내는 컴포넌트
* [흐름]
* DrawPhase -> PlanNextAction(TargetParty, FriendlyParty)
*				PickNext(): 패턴에서 다음 FEnemyAction 선택
*				PickTarget() : PartyComponent 프리미티브로 대상 결정
*				IntentComponent::SetIntent() 갱신
* EnemyPhase -> EmitActionEvent()
*				OnActionEmitted 브로드캐스트 → CombatManager ActionQueue 에 등록
* 
* [PartyComponent 와의 관계]
*	PlanNextAction 은 TArray<AUnit*> 대신 UPartyComponent* 를 받는다.
*	이유: PartyComponent 의 GetRandomAliveMember / GetLowestHPMember 등
*	타겟 선정 프리미티브를 재사용하기 위함.
*	alive 필터 람다를 내부에서 중복 구현하지 않는다.
* 
* 매개변수 명명 (적의 시점 기준):
*	TargetParty   = 공격·디버프 대상 파티 (플레이어 파티, CombatManager 의 AllyParty)
*	FriendlyParty = 자신의 파티 (적 그룹, CombatManager 의 EnemyParty)
* 
* [CombatManager 호출 예시 — 기존 PlanAllEnemyActions 교체]
*	for (AUnit* Enemy : EnemyParty->GetAliveMembers())
*	{
*       if (auto* Brain = Enemy->FindComponentByClass<UNPCBrainComponent>())
*           Brain->PlanNextAction(AllyParty, EnemyParty);
*   }
* 
* 
* [타겟 선정 고도화 확장 지점]
*	현재 SingleEnemy = 무작위 생존자. FEnemyAction 에 ETargetPreference 필드 추가 시
*	PickTarget 내부에서 GetLowestHPMember() 등으로 전환할 수 있다.
*/

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

	/**
	* 턴 시작 시 CombatManager 가 호출 → 행동 결정 + IntentComponent 갱신.
	*
	* @param TargetParty   공격·디버프 대상 파티 (플레이어 측 = AllyParty)
	* @param FriendlyParty 자신의 파티      (적 측    = EnemyParty)
	*/
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


//파티컴포넌트를 적용시킨다면 바꿔야 하고, 다른것을 사용한다면 그대로 변경없이 사용해도 무관