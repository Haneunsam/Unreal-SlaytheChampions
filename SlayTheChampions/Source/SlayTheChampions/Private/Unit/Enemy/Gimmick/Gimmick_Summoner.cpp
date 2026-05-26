// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/Enemy/Gimmick/Gimmick_Summoner.h"
#include "Unit/StatComponent.h"
#include "Unit/Unit.h"


void UGimmick_Summoner::BeginPlay()
{
	Super::BeginPlay();
}

void UGimmick_Summoner::OnGimmickTurnStart()
{
	++TurnsSinceLastSummon;

	//하수인이 없고 쿨타임 충족시 재소환
	const bool bNeedSummon = !bHasSummoned
		|| (!HasAliveMinion() && TurnsSinceLastSummon >= SummonCooldown);
	if (bNeedSummon)
	{
		OnMinionSpawnRequested.Broadcast(MinionCount);
		OnGimmickAnnounce.Broadcast(SummonAnnounce);
		TurnsSinceLastSummon = 0;
		bHasSummoned = true;
		return; //소환턴에는 힐 안함
	}

	//하수인이 있으면 보스 회복
	if (HasAliveMinion() && HealAmount > 0)
	{
		if (UStatComponent* Stat = GetOwner()->FindComponentByClass<UStatComponent>())
		{
			Stat->Heal(HealAmount);
		}
	}
}

void UGimmick_Summoner::RegisterMinion(AUnit* Minion)
{
	if (Minion)
	{
		SpawnedMinions.Add(Minion);
	}
}

bool UGimmick_Summoner::WillTriggerNextTurn() const
{
	return !bHasSummoned
		|| (!HasAliveMinion() && (TurnsSinceLastSummon + 1) >= SummonCooldown);
}

bool UGimmick_Summoner::HasAliveMinion() const
{
	return GetAliveMinionCount() > 0;
}

int32 UGimmick_Summoner::GetAliveMinionCount() const
{
	int32 Count = 0;
	for (const TWeakObjectPtr<AUnit>& Ref : SpawnedMinions)
	{
		if (Ref.IsValid() && Ref->IsAlive())
		{
			++Count;
		}
	}
	return Count;
}





