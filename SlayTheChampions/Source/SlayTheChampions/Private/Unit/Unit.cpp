// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/Unit.h"
#include "Unit/StatComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = false;
	// NotifyActorOnClicked 수신 조건: PlayerController BP에서 bEnableClickEvents = true 설정 필요

	// 클릭 감지 전용 캡슐을 루트로 생성
	// Visibility 채널만 Block — 물리/이동에는 영향 없이 마우스 클릭 반경만 확장
	// 클릭 감지 전용 캡슐 — 크기는 BP에서 유닛마다 조정
	ClickCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("ClickCapsule"));
	SetRootComponent(ClickCapsule);
	ClickCapsule->InitCapsuleSize(60.f, 100.f); // BP에서 유닛마다 덮어쓸 수 있는 기본값
	ClickCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ClickCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	ClickCapsule->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
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

void AUnit::NotifyAttack(bool bIsSkill)
{
	OnUnitAttackNotified.Broadcast(bIsSkill);
}

void AUnit::NotifyMove(FVector WorldDestination)
{
	OnUnitMoveNotified.Broadcast(WorldDestination);
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

	// StatComponent가 없으면 죽은 것으로 간주 (전투 대상에서 제외)
	return false;
}

// 마우스 클릭 시 OnUnitClicked 브로드캐스트
void AUnit::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	OnUnitClicked.Broadcast(this);
}