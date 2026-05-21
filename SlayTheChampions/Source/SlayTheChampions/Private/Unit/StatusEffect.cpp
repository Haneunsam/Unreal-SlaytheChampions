// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/StatusEffect.h"

void UStatusEffect::OnTurnEnd()
{
	if (Duration > 0)
	{
		--Duration;
	}
}
