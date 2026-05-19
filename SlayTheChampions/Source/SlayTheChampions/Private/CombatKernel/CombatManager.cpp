#include "CombatKernel/CombatManager.h"
#include "CombatKernel/CombatantActor.h"
#include "CombatKernel/CombatStatComponent.h"
#include "Components/BoxComponent.h"

ACombatManager::ACombatManager()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// 플레이어 슬롯 (파란색)
	SetupSlot(PlayerSlot_0, PlayerBox_0, TEXT("PlayerSlot_0"), TEXT("PlayerBox_0"), FVector(   0.f, -300.f, 0.f), FColor::Blue);
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
	SpawnedPlayers.Empty();
	SpawnedEnemies.Empty();

	USceneComponent* PlayerSlots[] = { PlayerSlot_0, PlayerSlot_1 };
	FCombatantInitData PlayerDataArr[] = { PlayerData_0, PlayerData_1 };

	const int32 ClampedPlayerCount = FMath::Clamp(PlayerCount, 1, 2);
	for (int32 i = 0; i < ClampedPlayerCount; i++)
	{
		ACombatantActor* Actor = SpawnCombatant(PlayerClass, PlayerSlots[i], PlayerDataArr[i]);
		if (Actor)
		{
			SpawnedPlayers.Add(Actor);
			UE_LOG(LogTemp, Warning, TEXT("[CombatManager] Player[%d] spawned at %s"), i, *PlayerSlots[i]->GetComponentLocation().ToString());
		}
	}

	USceneComponent* EnemySlots[] = { EnemySlot_0, EnemySlot_1, EnemySlot_2 };
	FCombatantInitData EnemyDataArr[] = { EnemyData_0, EnemyData_1, EnemyData_2 };

	const int32 ClampedEnemyCount = FMath::Clamp(EnemyCount, 1, 3);
	for (int32 i = 0; i < ClampedEnemyCount; i++)
	{
		ACombatantActor* Actor = SpawnCombatant(EnemyClass, EnemySlots[i], EnemyDataArr[i]);
		if (Actor)
		{
			SpawnedEnemies.Add(Actor);
			UE_LOG(LogTemp, Warning, TEXT("[CombatManager] Enemy[%d] spawned at %s"), i, *EnemySlots[i]->GetComponentLocation().ToString());
		}
	}
}

ACombatantActor* ACombatManager::SpawnCombatant(TSubclassOf<ACombatantActor> ActorClass,
                                                 USceneComponent* Slot,
                                                 const FCombatantInitData& Data)
{
	if (!ActorClass || !Slot) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ACombatantActor* Actor = GetWorld()->SpawnActor<ACombatantActor>(ActorClass, Slot->GetComponentTransform(), Params);
	if (!Actor) return nullptr;

	Actor->StatComponent->MaxHP     = Data.MaxHP;
	Actor->StatComponent->CurrentHP = Data.MaxHP;
	Actor->StatComponent->Defence   = Data.Defence;

	return Actor;
}

UCombatStatComponent* ACombatManager::GetPlayerStat(int32 Index) const
{
	if (!SpawnedPlayers.IsValidIndex(Index) || !SpawnedPlayers[Index]) return nullptr;
	return SpawnedPlayers[Index]->StatComponent;
}

UCombatStatComponent* ACombatManager::GetEnemyStat(int32 Index) const
{
	if (!SpawnedEnemies.IsValidIndex(Index) || !SpawnedEnemies[Index]) return nullptr;
	return SpawnedEnemies[Index]->StatComponent;
}
