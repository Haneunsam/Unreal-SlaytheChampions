// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/Enemy/EnemyInitializerComponent.h"
#include "Unit/StatComponent.h"
#include "Unit/Enemy/NPCBrainComponent.h"
#include "Unit/StatusEffectComponent.h"

// Sets default values for this component's properties
UEnemyInitializerComponent::UEnemyInitializerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyInitializerComponent::InitializeFromDefinition(UEnemyDefinitionData* Def)
{
    if (!Def) return;
    AActor* Owner = GetOwner();

    //스탯
    if (UStatComponent* Stat = Owner->FindComponentByClass<UStatComponent>())
    {
        Stat->MaxHP = Def->MaxHP; Stat->CurrentHP = Def->MaxHP;
    }

    //패턴
    if (UNPCBrainComponent* Brain = Owner->FindComponentByClass<UNPCBrainComponent>())
    {
        Brain->Pattern = Def->Pattern; Brain->SequenceIndex = 0;
    }

    //기믹
    if (Def->GimmickClass)
    {
        UGimmickComponent* G = NewObject<UGimmickComponent>(Owner, Def->GimmickClass);
        G->Data = Def->GimmickData;
        G->RegisterComponent(); // 동적 부착
    }

    //시작 상태이상
    if (UStatusEffectComponent* SE = Owner->FindComponentByClass<UStatusEffectComponent>())
        for (const FStartingEffect& E : Def->StartingEffects)
            SE->ApplyEffect(E.EffectClass, E.Stacks, E.Duration);
}

