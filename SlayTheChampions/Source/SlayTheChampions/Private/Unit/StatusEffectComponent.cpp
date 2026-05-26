//// Fill out your copyright notice in the Description page of Project Settings.
//
//
#include "Unit/StatusEffectComponent.h"
#include "Unit/Unit.h"
#include "Unit/StatusEffectmap.h"

// Sets default values for this component's properties
UStatusEffectComponent::UStatusEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


//효과 처리
void UStatusEffectComponent::AApplyEffect(EEffectType Type, int32 Value, int32 Duration)
{
    if (Type == EEffectType::None || !EffectMap) return;

    // 같은 종류 효과가 있으면 누적
    if (FStatusEffects* Existing = FFindEffect(Type))
    {
        Existing->Value += Value;
        ClampValue(*Existing);

        // 지속 턴은 더 긴 쪽으로 (-1은 무한, 무조건 우선)
        if (Existing->Count != -1)
        {
            if (Duration == -1)            Existing->Count = -1;
            else if (Duration > Existing->Count) Existing->Count = Duration;
        }
        
        //OnEffectValueChanged.Broadcast(*Existing)
        return;
    }
    // 새 효과: 매핑 DataAsset에서 템플릿 받아 빌드
    FStatusEffects NewEffect;
    if (!EffectMap->BuildRuntimeEffect(Type, Value, Duration, NewEffect))return;

    AActive.Add(NewEffect);
    //OnEffectApplied.Broadcast(NewEffect);
}

void UStatusEffectComponent::RRemoveEffect(EEffectType Type)
{
    for (int32 i = AActive.Num() - 1; i >= 0; --i)
    {
        if (AActive[i].EffectType == Type)
        {
            AActive.RemoveAt(i);
            //OnEffectRemoved.Broadcast(Type);
            // 같은 Type이 중복 추가될 일은 없지만, 안전하게 계속 순회
        }
    }
}

bool UStatusEffectComponent::HHasEffect(EEffectType Type) const
{
    return FFindEffect(Type) != nullptr;
}

int32 UStatusEffectComponent::GGetEffectValue(EEffectType Type) const
{
    const FStatusEffects* E = FFindEffect(Type);
    return E ? E->Value : 0;
}

FStatusEffects* UStatusEffectComponent::FFindEffect(EEffectType Type)
{
    for (FStatusEffects& E : AActive)
    {
        if (E.EffectType == Type) return &E;
    }
    return nullptr;
}

const FStatusEffects* UStatusEffectComponent::FFindEffect(EEffectType Type) const
{
    for (const FStatusEffects& E : AActive)
    {
        if (E.EffectType == Type) return &E;
    }
    return nullptr;
}

void UStatusEffectComponent::ClampValue(FStatusEffects& Effect) const
{
    if (Effect.Value < Effect.FloorValue) Effect.Value = Effect.FloorValue;
}

void UStatusEffectComponent::OnTurnStart()
{
    // bResetOnTurnStart == true인 효과를 즉시 제거.
    // 대표 예: Block(방어도). 자기 턴이 시작되면 무조건 사라진다.
    for (int32 i = AActive.Num() - 1; i >= 0; --i)
    {
        if (AActive[i].bResetOnTurnStart)
        {
            AActive.RemoveAt(i);
            //OnEffectRemoved.Broadcast(AActive[i].EffectType);
        }
    }
}

void UStatusEffectComponent::OnTurnEnd()
{
    // 뒤에서 앞으로 순회 (RemoveAt 시 인덱스 밀림 방지)
    for (int32 i = AActive.Num() - 1; i >= 0; --i)
    {
        FStatusEffects& E = AActive[i];

        // 1. DeltaPerTurn 적용
        //    작열(Burn): DefaultDeltaPerTurn = -1 → 매 턴 Value 1씩 감소
        //    재생(Regen): DefaultDeltaPerTurn = +N → 매 턴 Value N씩 증가 (힐 로직은 CombatManager가 Value를 읽어 처리)
        if (E.DeltaPerTurn != 0)
        {
            E.Value += E.DeltaPerTurn;
            ClampValue(E); // FloorValue 아래로 내려가지 않도록
        }

        // 2. Count 감소 (-1은 무한이므로 건너뜀)
        if (E.Count > 0) --E.Count;

        // 3. 만료 판정: Count가 0이 되면 제거
        if (E.Count == 0)
        {
            //OnEffectRemoved.Broadcast(E.EffectType);
            AActive.RemoveAt(i);
        }
    }
}




