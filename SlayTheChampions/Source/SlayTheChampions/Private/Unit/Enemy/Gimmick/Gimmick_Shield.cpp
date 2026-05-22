// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/Enemy/Gimmick/Gimmick_Shield.h"

// 사용방법
// 공격자가 TakeDamage 호출 이전에 이 함수를 먼저 통과시켜 사용
// 쉴드 있으면 데미지를 1로 반환하고, 쉴드 없으면 데미지를 그대로 반환하는 함수

// TakeDamage 호출 측 사용 체크
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


