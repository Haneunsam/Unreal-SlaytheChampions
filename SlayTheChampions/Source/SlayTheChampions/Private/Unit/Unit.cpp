// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/Unit.h"
#include "Unit/StatComponent.h"

// Sets default values
AUnit::AUnit()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AUnit::BeginPlay()
{
	Super::BeginPlay();
	
}

void AUnit::HandleDeath()
{
	OnUnitDied.Broadcast(this);
}

UStatComponent* AUnit::GetStat() const
{
	return FindComponentByClass<UStatComponent>();
}

bool AUnit::IsAlive() const
{
	if (const UStatComponent* Stat = GetStat())
	{
		return Stat->CurrentHP > 0;
	}

	//StatComponent가 없으면 일단 살아있다고 판단하지 않음
	return false;
}


