#include "CombatKernel/CombatManager.h"
#include "CombatKernel/CombatStatComponent.h"
#include "Components/BoxComponent.h"

ACombatManager::ACombatManager()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// 플레이어 슬롯 (파란색)
	SetupSlot(PlayerSlot_0, PlayerBox_0, TEXT("PlayerSlot_0"), TEXT("PlayerBox_0"), FVector(  0.f, -300.f, 0.f), FColor::Blue);
	SetupSlot(PlayerSlot_1, PlayerBox_1, TEXT("PlayerSlot_1"), TEXT("PlayerBox_1"), FVector(-150.f, -300.f, 0.f), FColor::Blue);

	// 적 슬롯 (빨간색)
	SetupSlot(EnemySlot_0, EnemyBox_0, TEXT("EnemySlot_0"), TEXT("EnemyBox_0"), FVector(   0.f, 300.f, 0.f), FColor::Red);
	SetupSlot(EnemySlot_1, EnemyBox_1, TEXT("EnemySlot_1"), TEXT("EnemyBox_1"), FVector(-150.f, 300.f, 0.f), FColor::Red);
	SetupSlot(EnemySlot_2, EnemyBox_2, TEXT("EnemySlot_2"), TEXT("EnemyBox_2"), FVector( 150.f, 300.f, 0.f), FColor::Red);
}

void ACombatManager::SetupSlot(USceneComponent*& OutSlot, UBoxComponent*& OutBox,
                                const FName& SlotName, const FName& BoxName,
                                const FVector& RelativeLocation, const FColor& Color)
{
	OutSlot = CreateDefaultSubobject<USceneComponent>(SlotName);
	OutSlot->SetupAttachment(GetRootComponent());
	OutSlot->SetRelativeLocation(RelativeLocation);

	OutBox = CreateDefaultSubobject<UBoxComponent>(BoxName);
	OutBox->SetupAttachment(OutSlot);
	OutBox->SetBoxExtent(FVector(40.f, 40.f, 80.f));
	OutBox->ShapeColor = Color;
	OutBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OutBox->SetHiddenInGame(true);
}

void ACombatManager::BeginPlay()
{
	Super::BeginPlay();
	InitCombat();
}

void ACombatManager::InitCombat()
{
	USceneComponent* PlayerSlots[] = { PlayerSlot_0, PlayerSlot_1 };
	for (int32 i = 0; i < PlayerActors.Num(); i++)
	{
		if (PlayerActors[i] && PlayerSlots[i])
		{
			PlayerActors[i]->SetActorLocation(PlayerSlots[i]->GetComponentLocation());
			UE_LOG(LogTemp, Warning, TEXT("[CombatManager] Player[%d] = %s"), i, *PlayerActors[i]->GetName());
		}
	}

	USceneComponent* EnemySlots[] = { EnemySlot_0, EnemySlot_1, EnemySlot_2 };
	for (int32 i = 0; i < EnemyActors.Num(); i++)
	{
		if (EnemyActors[i] && EnemySlots[i])
		{
			EnemyActors[i]->SetActorLocation(EnemySlots[i]->GetComponentLocation());
			UE_LOG(LogTemp, Warning, TEXT("[CombatManager] Enemy[%d] = %s"), i, *EnemyActors[i]->GetName());
		}
	}
}

UCombatStatComponent* ACombatManager::GetPlayerStat(int32 Index) const
{
	if (!PlayerActors.IsValidIndex(Index) || !PlayerActors[Index]) return nullptr;
	return PlayerActors[Index]->FindComponentByClass<UCombatStatComponent>();
}

UCombatStatComponent* ACombatManager::GetEnemyStat(int32 Index) const
{
	if (!EnemyActors.IsValidIndex(Index) || !EnemyActors[Index]) return nullptr;
	return EnemyActors[Index]->FindComponentByClass<UCombatStatComponent>();
}
