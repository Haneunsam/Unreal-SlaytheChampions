// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/Unit.h"
#include "Unit/StatComponent.h"

// Sets default values
AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = false;
	// NotifyActorOnClicked 수신 조건: PlayerController BP에서 bEnableClickEvents = true 설정 필요
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

	// StatComponent가 없으면 살아있다고 간주
	return false;
}

// 마우스 클릭 시 OnUnitClicked 브로드캐스트
void AUnit::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	OnUnitClicked.Broadcast(this);
}


