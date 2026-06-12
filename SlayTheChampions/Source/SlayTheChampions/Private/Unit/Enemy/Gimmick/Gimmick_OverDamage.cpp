// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/Enemy/Gimmick/Gimmick_OverDamage.h"


void UGimmick_OverDamage::OnGimmickTurnEnd()
{
	// CheckInterval 턴마다 체크 (첫 발동은 TurnCounter == CheckInterval)
	if(CheckInterval <= 0) return;
	if (TurnCounter > 0 && TurnCounter % CheckInterval == 0)
	{
		if (AccumulatedDamage < RequiredDamage)
		{
			// 분노 데미지 요청
			OnGimmickDamageRequest.Broadcast(ETargetType::AllEnemies, RageDamage);
			OnGimmickAnnounce.Broadcast(RageAnnounce);
		}
		AccumulatedDamage = 0;
	}
}

void UGimmick_OverDamage::HandleHPChanged(int32 OldHP, int32 NewHP)
{
	if (NewHP < OldHP)
	{
		AccumulatedDamage += (OldHP - NewHP);
	}
}


