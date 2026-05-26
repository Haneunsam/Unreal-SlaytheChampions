// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/Enemy/GimmickComponent.h"
#include "Unit/StatComponent.h"
#include "Unit/Unit.h"

// Sets default values for this component's properties
UGimmickComponent::UGimmickComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}



void UGimmickComponent::BeginPlay()
{
	Super::BeginPlay();

	
	// StatComponent.OnHPChanged 자동 바인딩
	if (UStatComponent* Stat = GetOwner()->FindComponentByClass<UStatComponent>())
	{
		Stat->OnHPChanged.AddDynamic(this, &UGimmickComponent::HandleHPChanged);
	}

	// Unit.OnUnitDied 자동 바인딩
	if (AUnit* Unit = Cast<AUnit>(GetOwner()))
	{
		Unit->OnUnitDied.AddDynamic(this, &UGimmickComponent::HandleOwnerDied);
	}

}

void UGimmickComponent::OnTurnStart()
{
	++TurnCounter;
	OnGimmickTurnStart();
}

void UGimmickComponent::OnTurnEnd()
{
	OnGimmickTurnEnd();
}

//Intent 보강 
//패턴이 만든 1차 Intent에 기믹 경고만 얹기
//기믹 실행은 OnGimmickDamageRequest등 기존 델리게이트가 담당
void UGimmickComponent::AugmentIntent(FIntent& InOutIntent) const
{
	if (WillTriggerNextTurn())
	{
		InOutIntent.bHasGimmickWarning = true;
		InOutIntent.GimmickWarningText = GimmickWarningText;
	}
}

