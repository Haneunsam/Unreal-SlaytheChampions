// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatKernel/CombatStatComponent.h"

UCombatStatComponent::UCombatStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatStatComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHP = MaxHP;
}

void UCombatStatComponent::TakeDamage(int32 Amount)
{
	if (Amount <= 0) return;

	// 방어도 먼저 소모
	int32 Remaining = Amount - Defence;
	Defence = FMath::Max(0, Defence - Amount);
	OnDefenceChanged.Broadcast(Defence);

	// 방어도로 못 막은 만큼 HP 차감
	if (Remaining > 0)
	{
		CurrentHP = FMath::Max(0, CurrentHP - Remaining);
		OnHPChanged.Broadcast(CurrentHP, MaxHP);
	}
}

void UCombatStatComponent::AddDefence(int32 Amount)
{
	if (Amount <= 0) return;
	Defence += Amount;
	OnDefenceChanged.Broadcast(Defence);
}

void UCombatStatComponent::Heal(int32 Amount)
{
	if (Amount <= 0) return;
	CurrentHP = FMath::Min(MaxHP, CurrentHP + Amount);
	OnHPChanged.Broadcast(CurrentHP, MaxHP);
}

void UCombatStatComponent::ResetDefence()
{
	Defence = 0;
	OnDefenceChanged.Broadcast(Defence);
}
