//// Fill out your copyright notice in the Description page of Project Settings.
//
//
#include "Unit/Enemy/NPCBrainComponent.h"
#include "Unit/Enemy/EnemyPatternData.h"
#include "Unit/Enemy/GimmickComponent.h"
#include "Unit/Unit.h"
#include "Unit/Enemy/IntentComponent.h"


// Sets default values for this component's properties
UNPCBrainComponent::UNPCBrainComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UNPCBrainComponent::PlanNextAction(const TArray<AUnit*>& Allies, const TArray<AUnit*>& Enemies)
{
	//행동결정
	//패턴이 없거나 패턴 액션이 없으면 바로 리턴
	if (!Pattern || Pattern->Actions.IsEmpty()) return;
	
	// 기믹 컴포넌트를 먼저 조회 (행동 억제 판정 + Intent 경고 보강, 두 군데에서 사용)
	UGimmickComponent* Gimmick = GetOwner()->FindComponentByClass<UGimmickComponent>();
	//Intent 구성
	FIntent New;
	if (Gimmick && Gimmick->SuppressesAction())
	{
		PendingAction = FEnemyAction();
		PendingAction.ActionType = EActionType::NoAttack;

		New.Kind = EIntentKind::NoAttack;
		New.TargetType = ETargetType::Self;
		// Value / Hits / Effect 등은 기본값(0 / 1 / None) 그대로
	}
	else
	{
		//PickNext,PickTarget private함수를 호출하여 다음 액션에 저장
		// 
		PendingAction = PickNext();
		AUnit* Target = PickTarget(PendingAction.TargetType, Allies, Enemies);

		New.Kind = PendingAction.ResolveIntentKind();
		New.Value = PendingAction.Value;
		New.Hits = PendingAction.Hits;
		New.TargetType = PendingAction.TargetType; //광역 단일 구분
		New.Target = Target;
		New.DisplayText = PendingAction.DisplayName;

		//행동에 딸린 상태효과를 Intent에 그대로 실어 보낸다.
		//CombatManager가 StatusEffectComponent::AApplyEffect(Type,Value,Duration)로 바로 사용.
		New.EffectType = PendingAction.EffectType;
		New.EffectValue = PendingAction.EffectValue;
		New.EffectDuration = PendingAction.EffectDuration;
	}
	//보스 등 GimmickComponent를 가진 적은 기믹 정보로 Intent 보강
	//일반 적은 nullptr로 스킵됨
	if (Gimmick)
	{
		Gimmick->AugmentIntent(New);
	}
	//완성된 Intent를 저장 (EmitActionEvent가 이것을 송출)
	PendingIntent = New;

	if (UIntentComponent* Intent = GetOwner()->FindComponentByClass<UIntentComponent>())
	{
		Intent->SetIntent(New);
	}
	

}

void UNPCBrainComponent::EmitActionEvent()
{
	//ActionQueue로 송출
	OnActionEmitted.Broadcast(PendingIntent);
}

//sequential 코드리뷰

FEnemyAction UNPCBrainComponent::PickNext()
{
	const auto& Actions = Pattern->Actions;

	//Sequential인지
	if (Pattern->Mode == EPatternMode::Sequential)
	{
		FEnemyAction Picked = Actions[SequenceIndex % Actions.Num()];
		++SequenceIndex;
		if (!Pattern->bLoop && SequenceIndex >= Actions.Num())
		{
			SequenceIndex = Actions.Num() - 1;
		}
		return Picked;
	}
	//나머지인 weighted 분기
	else 
	{
		float Total = 0.f;
		for (const FEnemyAction& A : Actions) Total += FMath::Max(0.f, A.Weight);
		float Roll = FMath::FRandRange(0.f, Total);
		for (const FEnemyAction& A : Actions)
		{
			Roll -= A.Weight;
			if (Roll <= 0.f) return A;
		}
		return Actions.Last();
	}
	
}

AUnit* UNPCBrainComponent::PickTarget(
	ETargetType TargetType,
	const TArray<AUnit*>& Allies,
	const TArray<AUnit*>& Enemies) const
{
	auto Alive = [](const TArray<AUnit*>& In)
		{
			TArray<AUnit*> Out;
			for (AUnit* U : In) { if (U && U->IsAlive()) Out.Add(U); }
			return Out;
		};
	switch (TargetType)
	{
	case ETargetType::Self:
		return Cast<AUnit>(GetOwner());
	case ETargetType::SingleEnemy:
	{
		TArray<AUnit*> A = Alive(Enemies);
		return A.IsEmpty() ? nullptr : A[FMath::RandRange(0, A.Num() - 1)];
	}
	case ETargetType::RamdomAlly:
	{
		TArray<AUnit*> A = Alive(Allies);
		return A.IsEmpty() ? nullptr : A[FMath::RandRange(0, A.Num() - 1)];
	}
	//광역은 단일 타겟이 없음 CombatManager가 전체 적용
	default:
		return nullptr;

	}

	
}

/*
void UNPCBrainComponent::SwapPattern(UEnemyPatternData* NewPattern)
{
	Pattern = NewPattern;
	SequenceIndex = 0;
}
*/



