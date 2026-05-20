#include "CombatKernel/CombatManager.h"
#include "Unit/Unit.h"
#include "Unit/StatComponent.h"
#include "Components/BoxComponent.h"

ACombatManager::ACombatManager()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// 플레이어 박스 (파란색) - 에디터에서 직접 이동하여 스폰 위치 조정
	PlayerBox_0 = SetupBox(TEXT("PlayerBox_0"), FVector(   0.f, -300.f, 0.f), FColor::Blue);
	PlayerBox_1 = SetupBox(TEXT("PlayerBox_1"), FVector(-150.f, -300.f, 0.f), FColor::Blue);

	// 적 박스 (빨간색) - 에디터에서 직접 이동하여 스폰 위치 조정
	EnemyBox_0 = SetupBox(TEXT("EnemyBox_0"), FVector(   0.f, 300.f, 0.f), FColor::Red);
	EnemyBox_1 = SetupBox(TEXT("EnemyBox_1"), FVector(-150.f, 300.f, 0.f), FColor::Red);
	EnemyBox_2 = SetupBox(TEXT("EnemyBox_2"), FVector( 150.f, 300.f, 0.f), FColor::Red);
}

UBoxComponent* ACombatManager::SetupBox(const FName& BoxName,
                                         const FVector& RelativeLocation,
                                         const FColor& Color)
{
	UBoxComponent* Box = CreateDefaultSubobject<UBoxComponent>(BoxName);
	Box->SetupAttachment(GetRootComponent());
	Box->SetRelativeLocation(RelativeLocation);
	Box->SetBoxExtent(FVector(40.f, 40.f, 80.f));
	Box->ShapeColor = Color;
	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Box->SetHiddenInGame(true);
	return Box;
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

	UBoxComponent* PlayerBoxes[] = { PlayerBox_0, PlayerBox_1 };
	FCombatantInitData PlayerDataArr[] = { PlayerData_0, PlayerData_1 };

	const int32 ClampedPlayerCount = FMath::Clamp(PlayerCount, 1, 2);
	for (int32 i = 0; i < ClampedPlayerCount; i++)
	{
		AUnit* Actor = SpawnCombatant(PlayerClass, PlayerBoxes[i], PlayerDataArr[i]);
		if (Actor)
		{
			SpawnedPlayers.Add(Actor);
			UE_LOG(LogTemp, Warning, TEXT("[CombatManager] Player[%d] spawned at %s"), i, *PlayerBoxes[i]->GetComponentLocation().ToString());
		}
	}

	UBoxComponent* EnemyBoxes[] = { EnemyBox_0, EnemyBox_1, EnemyBox_2 };
	FCombatantInitData EnemyDataArr[] = { EnemyData_0, EnemyData_1, EnemyData_2 };

	const int32 ClampedEnemyCount = FMath::Clamp(EnemyCount, 1, 3);
	for (int32 i = 0; i < ClampedEnemyCount; i++)
	{
		AUnit* Actor = SpawnCombatant(EnemyClass, EnemyBoxes[i], EnemyDataArr[i]);
		if (Actor)
		{
			SpawnedEnemies.Add(Actor);
			UE_LOG(LogTemp, Warning, TEXT("[CombatManager] Enemy[%d] spawned at %s"), i, *EnemyBoxes[i]->GetComponentLocation().ToString());
		}
	}
}

AUnit* ACombatManager::SpawnCombatant(TSubclassOf<AUnit> ActorClass,
                                      UBoxComponent* Box,
                                      const FCombatantInitData& Data)
{
	if (!ActorClass || !Box) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 박스의 월드 트랜스폼을 스폰 위치로 사용
	AUnit* Actor = GetWorld()->SpawnActor<AUnit>(ActorClass, Box->GetComponentTransform(), Params);
	if (!Actor) return nullptr;

	// StatComponent는 Blueprint에서 추가되므로 없을 수 있음
	UStatComponent* Stat = Actor->GetStat();
	if (Stat)
	{
		Stat->MaxHP     = Data.MaxHP;
		Stat->CurrentHP = Data.MaxHP;
		// [임시] Defence 주입은 UStatComponent에 방어도 추가 후 처리
	}

	return Actor;
}

void ACombatManager::ExecuteCard(const FCardDataRow& Card, int32 CasterIndex)
{
	if (!SpawnedPlayers.IsValidIndex(CasterIndex)) return;
	AUnit* Caster = SpawnedPlayers[CasterIndex];

	// 타겟 목록 결정
	TArray<AUnit*> Targets;
	switch (Card.TargetType)
	{
		case ETargetType::SingleEnemy:
			// [임시] 선택 UI 없으므로 0번 적 고정
			if (SpawnedEnemies.IsValidIndex(0)) Targets.Add(SpawnedEnemies[0]);
			break;
		case ETargetType::AllEnemies:
			Targets = SpawnedEnemies;
			break;
		case ETargetType::Self:
			Targets.Add(Caster);
			break;
		case ETargetType::SingleAlly:
			// [임시] 0번 플레이어 고정
			if (SpawnedPlayers.IsValidIndex(0)) Targets.Add(SpawnedPlayers[0]);
			break;
		case ETargetType::AllAllies:
		case ETargetType::Single_Team:
			Targets = SpawnedPlayers;
			break;
	}

	// 효과 실행
	for (AUnit* Target : Targets)
	{
		if (!Target || !Target->IsAlive()) continue;

		UStatComponent* Stat = Target->GetStat();
		if (!Stat) continue;

		// 데미지 (UsingCount 횟수만큼 반복)
		if (Card.Damage > 0)
		{
			for (int32 i = 0; i < Card.UsingCount; i++)
				Stat->TakeDamage(Card.Damage, Caster);
		}

		// 회복
		if (Card.HealAmount > 0)
			Stat->Heal(Card.HealAmount);

		// [임시] Block — UStatComponent에 방어도 추가 시 구현
		// [임시] EffectTag — StatusEffectComponent 연결 시 구현
	}
}

UStatComponent* ACombatManager::GetPlayerStat(int32 Index) const
{
	if (!SpawnedPlayers.IsValidIndex(Index) || !SpawnedPlayers[Index]) return nullptr;
	return SpawnedPlayers[Index]->GetStat();
}

UStatComponent* ACombatManager::GetEnemyStat(int32 Index) const
{
	if (!SpawnedEnemies.IsValidIndex(Index) || !SpawnedEnemies[Index]) return nullptr;
	return SpawnedEnemies[Index]->GetStat();
}
