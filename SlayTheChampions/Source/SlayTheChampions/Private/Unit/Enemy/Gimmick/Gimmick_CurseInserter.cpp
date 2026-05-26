// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/Enemy/Gimmick/Gimmick_CurseInserter.h"

bool UGimmick_CurseInserter::WillTriggerNextTurn() const
{
	const int32 T = GetPredictedTurn();
	return InsertInterval > 0 && T > 0 && (T % InsertInterval == 0);
}

void UGimmick_CurseInserter::OnGimmickTurnStart()
{
	if (InsertInterval <= 0) return;
	if (TurnCounter > 0 && TurnCounter % InsertInterval == 0)
	{
		OnCurseInsertRequested.Broadcast(CurseCardID);
		++TotalCursesInserted;
		OnGimmickAnnounce.Broadcast(InsertAnnounce);
	}
}
