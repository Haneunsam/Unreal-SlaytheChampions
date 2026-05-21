// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/Enemy/Gimmick/Gimmick_CurseInserter.h"

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
