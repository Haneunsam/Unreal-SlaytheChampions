// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/Enemy/Gimmick/Gimmick_Shield.h"

// 동작원리
// 공격자가 TakeDamage호출 전에 이 함수로 데미지 보정
// 스택이 있으면 1을 반환하고 , 없으면 원래 데미지 그대로 들어가는 함수

// TakeDamage 직전에 방패 체크
/*
int32 FinalDamage = RawDamage;
if (UGimmick_AbsorptionShield* Shield = Target->FindComponentByClass<UGimmick_AbsorptionShield>())
{
	FinalDamage = Shield->ProcessIncomingDamage(RawDamage);
}
Target->GetStat()->TakeDamage(FinalDamage, Instigator);
*/

void UGimmick_Shield::BeginPlay()
{
	Super::BeginPlay();
	CurrentStacks = MaxStacks;
}


int32 UGimmick_Shield::ProcessIncomingDamage(int32 RawDamage)
{
	if (CurrentStacks < 0) return RawDamage;
	const int32 Old = CurrentStacks;
	--CurrentStacks;
	OnShieldStackChanged.Broadcast(Old, CurrentStacks);
	return 1;
}


