// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/StatusEffectmap.h"

const FStatusEffectTemplate* UStatusEffectmap::Resolve(EEffectType Type) const
{
	return Templates.Find(Type);
}

bool UStatusEffectmap::BuildRuntimeEffect(EEffectType Type, int32 Value, int32 Duration,
	FStatusEffects& OutEffect) const
{
    if (Type == EEffectType::None) return false;

    const FStatusEffectTemplate* Tpl = Resolve(Type);
    if (!Tpl) return false;

    OutEffect = FStatusEffects{};
    OutEffect.EffectType = Type;
    OutEffect.Value = Value;
    OutEffect.Count = Duration;
    OutEffect.DeltaPerTurn = Tpl->DefaultDeltaPerTurn;
    OutEffect.FloorValue = Tpl->DefaultFloorValue;
    OutEffect.bResetOnTurnStart = Tpl->bDefaultResetOnTurnStart;

    // 초기값 클램프
    if (OutEffect.Value < OutEffect.FloorValue) OutEffect.Value = OutEffect.FloorValue;

    return true;
	
}

UTexture2D* UStatusEffectmap::GetIcon(EEffectType Type) const
{
    const FStatusEffectTemplate* Tpl = Resolve(Type);
    return Tpl ? Tpl->Icon : nullptr;
}

FText UStatusEffectmap::GetDisplayName(EEffectType Type) const
{
    const FStatusEffectTemplate* Tpl = Resolve(Type);
    return Tpl ? Tpl->DisplayName : FText::GetEmpty();
}
