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

    // ���� ȿ���� �̹� ������ ���� ����
    if (UStatusEffect* Existing = FindEffect(EffectClass))
    {
        Existing->Stacks += Stacks;
        if (Duration > Existing->Duration) Existing->Duration = Duration;
        OnEffectApplied.Broadcast(Existing);
        return Existing;
    }
    // ���ο� ȿ�� ����
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
    //������ ��ȸ�ϸ鼭 ã��
    for (int32 i = Active.Num() - 1; i >= 0; --i)
    {
        if (Active[i] && Active[i]->IsA(EffectClass))
        {
            //�迭�� ���� �ּҷ� �����Ͽ� StatusEffect.cpp�� �ִ� OnRemoved ����
            //���� �Ƹ�  ����� ��ġ�Ͽ� �ƹ��͵� ���� ���ҵ�
            Active[i]->OnRemoved();
            //Active�迭�� �ִ� �ش� �����̻� ����
            Active.RemoveAt(i);
            //��ε�ĳ��Ʈ
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

void UStatusEffectComponent::SetEffectValue(EEffectType Type, int32 NewValue)
{
	const int32 OldValue = GetEffectValue(Type);
	EffectValues.Add(Type, NewValue);
	OnEffectValueChanged.Broadcast(Type, OldValue, NewValue);
}

int32 UStatusEffectComponent::GetEffectValue(EEffectType Type) const
{
	const int32* Found = EffectValues.Find(Type);
	return Found ? *Found : 0;
}



