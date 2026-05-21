// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/StatusEffectComponent.h"
#include "Unit/Unit.h"

// Sets default values for this component's properties
UStatusEffectComponent::UStatusEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

UStatusEffect* UStatusEffectComponent::ApplyEffect(
	TSubclassOf<UStatusEffect> EffectClass, int32 Stacks, int32 Duration)
{
    if (!EffectClass || Stacks <= 0) return nullptr;

    // 같은 효과가 이미 존재하면 스택 합산
    if (UStatusEffect* Existing = FindEffect(EffectClass))
    {
        Existing->Stacks += Stacks;
        if (Duration > Existing->Duration) Existing->Duration = Duration;
        OnEffectApplied.Broadcast(Existing);
        return Existing;
    }
    // 새로운 효과 생성
    UStatusEffect* New = NewObject<UStatusEffect>(this, EffectClass);
    New->Stacks = Stacks;
    New->Duration = Duration;
    New->Owner = Cast<AUnit>(GetOwner());
    New->OnApplied();

    Active.Add(New);
    OnEffectApplied.Broadcast(New);
    return New;
}

void UStatusEffectComponent::RemoveEffect(TSubclassOf<UStatusEffect> EffectClass)
{
    // 역순으로 순회하면서 찾기
    for (int32 i = Active.Num() - 1; i >= 0; --i)
    {
        if (Active[i] && Active[i]->IsA(EffectClass))
        {
            // 배열의 마지막 주소로 덮어써서 StatusEffect.cpp에 있는 OnRemoved 호출
            // 만약 아마 이렇게 작성하여 아무것도 없이 제거됨
            Active[i]->OnRemoved();
            // Active 배열에 있는 해당 상태이상 제거
            Active.RemoveAt(i);
            // 브로드캐스트
            OnEffectRemoved.Broadcast(EffectClass);
        }
    }
}

UStatusEffect* UStatusEffectComponent::FindEffect(TSubclassOf<UStatusEffect> EffectClass) const
{
	for (UStatusEffect* E : Active)
	{
		if (E && E->IsA(EffectClass)) return E;
	}
	return nullptr;
}



