// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/Enemy/NPCBrainComponent.h"
#include "Unit/Enemy/EnemyPatternData.h"
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
	//[기믹 상태] 조회만 -구동은 CombatManger책임
	UGimmickComponent* Gimmick = GetOwner()->FindComponentByClass<UGimmickComponent>();
	//기믹이 이번턴 행동을 막으면
	if (Gimmick && Gimmick->WantsToSkipPatternAction())
	{
		PendingAction = FEnemyAction{};
		PendingAction.IntentKind = EIntentKind::NoAttack;
		if (UIntentComponent* Intent = GetOwner()->FindComponentByClass<UIntentComponent>())
		{
			FIntent New; New.Kind = EIntentKind::NoAttack;
			Intent->SetIntent(New);
		}
		return;
	}
	// 행동 계획
	// 패턴이 없거나 비어있는 액션이 없으면 바로 종료
	if (!Pattern || Pattern->Actions.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[NPCBrain] %s — Pattern이 없거나 비어있음. PendingAction이 기본값으로 유지됩니다."), *GetOwner()->GetName());
		return;
	}
		
	// PickNext, PickTarget private 함수를 호출하여 다음 액션에 반영
	//
	PendingAction = PickNext();
	AUnit* Target = PickTarget(PendingAction.TargetType, Allies, Enemies);

	// IntentComponent 갱신
	if (UIntentComponent* Intent = GetOwner()->FindComponentByClass<UIntentComponent>())
	{
		FIntent New;
		New.Kind = PendingAction.IntentKind;
		New.Value = PendingAction.Value;
		New.Hits = PendingAction.Hits;
		New.Target = Target;
		New.DisplayText = PendingAction.DisplayName;
		Intent->SetIntent(New);
	}
}

void UNPCBrainComponent::EmitActionEvent()
{
	// ActionQueue에 발행
	OnActionEmitted.Broadcast(PendingAction);
}

// sequential 순환 루프

FEnemyAction UNPCBrainComponent::PickNext()
{
	const auto& Actions = Pattern->Actions;

	// Sequential 모드
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
	// 순차가 아닌 weighted 뽑기
	else
	{
		float Total = 0.f;
		for (const FEnemyAction& A : Actions) Total += FMath::Max(0.f, A.Weight);

		// 모든 weight가 0 이하거나 전부 동일하면 균등 랜덤 선택
		const float FirstW = FMath::Max(0.f, Actions[0].Weight);
		bool bAllEqual = true;
		for (const FEnemyAction& A : Actions)
		{
			if (!FMath::IsNearlyEqual(FMath::Max(0.f, A.Weight), FirstW))
			{
				bAllEqual = false;
				break;
			}
		}
		if (Total <= 0.f || bAllEqual)
			return Actions[FMath::RandRange(0, Actions.Num() - 1)];

		// 가중치 기반 랜덤 선택
		float Roll = FMath::FRandRange(0.f, Total);
		for (const FEnemyAction& A : Actions)
		{
			Roll -= FMath::Max(0.f, A.Weight);
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
	case ETargetType::SingleAlly:
	{
		TArray<AUnit*> A = Alive(Allies);
		return A.IsEmpty() ? nullptr : A[FMath::RandRange(0, A.Num() - 1)];
	}
	// 나머지 광역 타겟은 CombatManager에서 전체 처리
	default:
		return nullptr;

	}


}
