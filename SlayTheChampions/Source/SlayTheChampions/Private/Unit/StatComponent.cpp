// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/StatComponent.h"
#include "Unit/Unit.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


void UStatComponent::TakeDamage(int32 Damage, AUnit* Instigator)
{
	//데미지가 들어와 HP가 다는 함수 구현부
	//데미지 0이거나 현재 HP가 0이하이면 리턴
	if (Damage <= 0 || CurrentHP <= 0) return;

	//HP 차감
	const int32 OldHP = CurrentHP;
	CurrentHP = FMath::Max(0, CurrentHP - Damage);

	//브로드캐스트
	OnHPChanged.Broadcast(OldHP, CurrentHP);

	//HP가 0이라면
	if (CurrentHP <= 0)
	{
		if (AUnit* Unit = Cast<AUnit>(GetOwner()))
		{
			Unit->HandleDeath();
		}
	}
}

void UStatComponent::Heal(int32 Amount)
{
	//힐이 들어와 체력이 차는 함수 구현부
	//힐량이 0이거나, 현재체력이 0이거나, 최대체력 이상일때
	if (Amount <= 0 || CurrentHP <= 0 || CurrentHP >= MaxHP) return;

	const int32 OldHP = CurrentHP;
	CurrentHP = FMath::Min(MaxHP, CurrentHP + Amount);
	if (OldHP != CurrentHP)
	{
		OnHPChanged.Broadcast(OldHP, CurrentHP);
	}
}

// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHP = MaxHP;
	// ...
	
}


