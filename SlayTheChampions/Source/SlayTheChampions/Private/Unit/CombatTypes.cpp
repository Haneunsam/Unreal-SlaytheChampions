// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/CombatTypes.h"

// ──────────────────────────────────────────────
//  ActionType + EffectType → 최종 EIntentKind 도출.
//  디자이너는 ActionType(공격/보호막/무행동)만 고르고,
//  버프/디버프 의도는 이 함수가 EffectType을 보고 자동으로 결정한다.
//  여기서 EffectTypeHelpers가 처음으로 실제 사용된다.
// ──────────────────────────────────────────────
EIntentKind FEnemyAction::ResolveIntentKind() const
{
    switch (ActionType)
    {
    case EActionType::Attack:
        return EIntentKind::Attack;

    case EActionType::Defend:
        return EIntentKind::Defend;

    case EActionType::NoAttack:
        // 무행동이라도 효과가 붙어 있으면 그 효과의 성격(Buff/Debuff)을 의도로 삼는다.
        if (EffectType != EEffectType::None)
        {
            return EffectTypeHelpers::ToIntentKind(EffectType);
        }
        return EIntentKind::NoAttack;

    default:
        return EIntentKind::Unknown;
    }
}