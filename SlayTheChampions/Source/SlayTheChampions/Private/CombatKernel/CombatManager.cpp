#include "CombatKernel/CombatManager.h"
#include "CombatKernel/EffectManager.h"
#include "CombatKernel/CombatStatWidget.h"
#include "CombatKernel/BattleMainWidget.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Unit/Unit.h"
#include "Unit/StatComponent.h"
#include "Unit/StatusEffectComponent.h"
#include "Unit/StatusEffect.h"
#include "Unit/Enemy/NPCBrainComponent.h"
#include "Unit/Enemy/IntentComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/WidgetComponent.h"
#include "Card/CardUserComponent.h"  // 스폰된 플레이어에 PawnIndex 주입 및 드로우 호출용

// 생성자: 스폰 위치 박스 컴포넌트들을 미리 배치
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

	// 카메라 슬롯 (화살표 = 카메라 방향) - 에디터에서 이동·회전하여 위치·시선 조정
	// Default: 흰색 — 전투 시작 초기 위치
	CameraSlot_Default = CreateDefaultSubobject<UArrowComponent>(TEXT("CameraSlot_Default"));
	CameraSlot_Default->SetupAttachment(GetRootComponent());
	CameraSlot_Default->SetRelativeLocation(FVector(-500.f, 0.f, 200.f));
	CameraSlot_Default->ArrowColor = FColor::White;
	CameraSlot_Default->SetHiddenInGame(true);

	// Player_0: 파란색 — 0번 플레이어 선택 시 이동 위치
	CameraSlot_Player_0 = CreateDefaultSubobject<UArrowComponent>(TEXT("CameraSlot_Player_0"));
	CameraSlot_Player_0->SetupAttachment(GetRootComponent());
	CameraSlot_Player_0->SetRelativeLocation(FVector(-500.f, -200.f, 200.f));
	CameraSlot_Player_0->ArrowColor = FColor::Blue;
	CameraSlot_Player_0->SetHiddenInGame(true);

	// Player_1: 하늘색 — 1번 플레이어 선택 시 이동 위치
	CameraSlot_Player_1 = CreateDefaultSubobject<UArrowComponent>(TEXT("CameraSlot_Player_1"));
	CameraSlot_Player_1->SetupAttachment(GetRootComponent());
	CameraSlot_Player_1->SetRelativeLocation(FVector(-500.f, -350.f, 200.f));
	CameraSlot_Player_1->ArrowColor = FColor::Cyan;
	CameraSlot_Player_1->SetHiddenInGame(true);

	// Enemy: 빨간색 — 타겟 지정 시 이동 위치 (적 앞)
	CameraSlot_Enemy = CreateDefaultSubobject<UArrowComponent>(TEXT("CameraSlot_Enemy"));
	CameraSlot_Enemy->SetupAttachment(GetRootComponent());
	CameraSlot_Enemy->SetRelativeLocation(FVector(-500.f, 300.f, 200.f));
	CameraSlot_Enemy->ArrowColor = FColor::Red;
	CameraSlot_Enemy->SetHiddenInGame(true);
}

// 스폰 위치 박스 컴포넌트 하나를 생성하고 루트에 부착하는 헬퍼 함수
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

// 게임 시작 시 전투를 초기화
void ACombatManager::BeginPlay()
{
	Super::BeginPlay();
	InitCombat();
}

// 플레이어/적 유닛을 스폰하고 1턴을 시작
void ACombatManager::InitCombat()
{
	SpawnedPlayers.Empty();
	SpawnedEnemies.Empty();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();

	// ── 1. 배틀 메인 위젯 생성 ────────────────────────────────
	// NativeConstruct에서 레벨의 CombatManager를 자동 탐색해 바인딩하므로 AddToViewport 전 CombatManager가 존재해야 함
	if (BattleWidgetClass && PC)
	{
		BattleWidget = CreateWidget<UBattleMainWidget>(PC, BattleWidgetClass);
		if (BattleWidget)
			BattleWidget->AddToViewport();
		else
			UE_LOG(LogTemp, Error, TEXT("[CombatManager] BattleWidget creation failed"));
	}

	// ── 2. 배틀 카메라 스폰 ──────────────────────────────────
	// CameraSlot_Default 화살표 위치·회전으로 스폰 — 에디터에서 화살표를 이동·회전해 초기 위치 조정
	if (BattleCameraClass)
	{
		const FTransform SpawnTransform = CameraSlot_Default
			? CameraSlot_Default->GetComponentTransform()
			: GetActorTransform();

		BattleCamera = GetWorld()->SpawnActor<ACameraActor>(BattleCameraClass, SpawnTransform);
		if (BattleCamera && PC)
			PC->SetViewTargetWithBlend(BattleCamera);
		else
			UE_LOG(LogTemp, Error, TEXT("[CombatManager] BattleCamera spawn failed"));
	}

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

			// PawnIndex 주입 후 덱 재초기화 (BP 기본값 0을 덮어씀)
			UCardUserComponent* CardComp = Actor->FindComponentByClass<UCardUserComponent>();
			if (CardComp)
			{
				CardComp->PawnIndex = i;
				CardComp->InitializeDeck();
			}
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

	StartTurn();
}

// 지정된 클래스의 유닛을 박스 위치에 스폰하고 초기 스탯과 위젯을 설정
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

	// CombatStatWidget 자동 연결
	UWidgetComponent* WidgetComp = Actor->FindComponentByClass<UWidgetComponent>();
	if (!WidgetComp)
	{
		UE_LOG(LogTemp, Error, TEXT("[SpawnCombatant] %s — WidgetComponent 없음"), *Actor->GetName());
	}
	else
	{
		UCombatStatWidget* StatWidget = Cast<UCombatStatWidget>(WidgetComp->GetUserWidgetObject());
		if (!StatWidget)
		{
			UE_LOG(LogTemp, Error, TEXT("[SpawnCombatant] %s — GetUserWidgetObject 실패 (위젯 클래스가 CombatStatWidget이 아니거나 아직 초기화 안 됨)"), *Actor->GetName());
		}
		else
		{
			StatWidget->InitFromUnit(Actor);
			UE_LOG(LogTemp, Warning, TEXT("[SpawnCombatant] %s — StatWidget 연결 성공"), *Actor->GetName());
		}
	}

	return Actor;
}

// 카드 데이터를 기반으로 타겟을 결정하고 데미지/회복/방어도 효과를 적용
// TargetOverride가 있으면 SingleEnemy 타입에서 해당 유닛을 타겟으로 우선 사용
void ACombatManager::ExecuteCard(const FCardDataRow& Card, int32 CasterIndex, AUnit* TargetOverride)
{
	if (!SpawnedPlayers.IsValidIndex(CasterIndex)) return;
	AUnit* Caster = SpawnedPlayers[CasterIndex];

	// 타겟 목록 결정
	TArray<AUnit*> Targets;
	switch (Card.TargetType)
	{
		case ETargetType::SingleEnemy:
			// 플레이어가 선택한 타겟 우선, 없으면 0번 적 고정
			if (TargetOverride) Targets.Add(TargetOverride);
			else if (SpawnedEnemies.IsValidIndex(0)) Targets.Add(SpawnedEnemies[0]);
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

	UE_LOG(LogTemp, Warning, TEXT("[ExecuteCard] Targets=%d Damage=%d"), Targets.Num(), Card.Damage);

	// ── 1. 기본 효과: Damage / HealAmount / Block — 카드의 TargetType 대상에게 적용 ──
	for (AUnit* Target : Targets)
	{
		if (!Target || !Target->IsAlive()) continue;

		UStatComponent* Stat = Target->GetStat();
		if (!Stat) continue;

		if (Card.Damage > 0)
		{
			for (int32 i = 0; i < Card.UsingCount; i++)
				UEffectManager::ProcessDamage(Target, Card.Damage, Caster);
		}

		if (Card.HealAmount > 0)
			Stat->Heal(Card.HealAmount);

		if (Card.Block > 0)
			UEffectManager::ApplyEffect(Target, EEffectType::Shield, Card.Block);
	}

	// ── 2. Effects 배열 — 각 항목의 TargetType으로 독립적으로 대상 결정 ──────────
	// EEffectTargetType::UseCardDefault이면 카드 기본 Targets 그대로 사용
	auto ResolveEffectTargets = [&](EEffectTargetType EffTargetType) -> TArray<AUnit*>
	{
		switch (EffTargetType)
		{
			case EEffectTargetType::UseCardDefault: return Targets;
			case EEffectTargetType::SingleEnemy:
				if (TargetOverride) return { TargetOverride };
				if (SpawnedEnemies.IsValidIndex(0)) return { SpawnedEnemies[0] };
				return {};
			case EEffectTargetType::AllEnemies:   return SpawnedEnemies;
			case EEffectTargetType::Self:          return { Caster };
			case EEffectTargetType::SingleAlly:
				if (SpawnedPlayers.IsValidIndex(0)) return { SpawnedPlayers[0] };
				return {};
			case EEffectTargetType::AllAllies:     return SpawnedPlayers;
			default:                               return Targets;
		}
	};

	for (const FCardEffect& Effect : Card.Effects)
	{
		if (Effect.EffectType == EEffectType::None || Effect.Value <= 0) continue;

		const TArray<AUnit*> EffectTargets = ResolveEffectTargets(Effect.TargetType);
		const uint8 TypeVal = static_cast<uint8>(Effect.EffectType);

		for (AUnit* Target : EffectTargets)
		{
			if (!Target || !Target->IsAlive()) continue;

			if (TypeVal >= 200)
				UEffectManager::ApplyDebuff(Target, Effect.EffectType, Effect.Value);
			else if (TypeVal >= 100)
				UEffectManager::ApplyBuff(Target, Effect.EffectType, Effect.Value);
			else
				UEffectManager::ApplyEffect(Target, Effect.EffectType, Effect.Value);
		}
	}

	// 카드 실행 완료 브로드캐스트 (히스토리 위젯·애니메이션 트리거용)
	OnActionExecuted.Broadcast(Card, CasterIndex);
}

// 모든 적 또는 플레이어의 생존 여부를 확인하고 결과를 로그로 출력
// TODO: 승리/패배 화면 구현 시 여기서 페이즈 진행 중단 처리 추가
void ACombatManager::CheckCombatEnd()
{
	// 살아있는 적이 한 명도 없으면 전투 승리
	const bool bAllEnemiesDead = SpawnedEnemies.Num() > 0 &&
		!SpawnedEnemies.ContainsByPredicate([](AUnit* U){ return U && U->IsAlive(); });

	if (bAllEnemiesDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CombatManager] 모든 적 사망 — 전투 승리"));
		// TODO: 전투 종료 처리 (승리 화면, 보상 등) 구현 시 여기서 페이즈 진행 중단
		return;
	}

	// 살아있는 플레이어가 한 명도 없으면 게임 오버
	const bool bAllPlayersDead = SpawnedPlayers.Num() > 0 &&
		!SpawnedPlayers.ContainsByPredicate([](AUnit* U){ return U && U->IsAlive(); });

	if (bAllPlayersDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CombatManager] 모든 플레이어 사망 — 게임 오버"));
		// TODO: 게임 오버 처리 (패배 화면 등) 구현 시 여기서 페이즈 진행 중단
	}
}

// 현재 페이즈를 변경하고 델리게이트를 브로드캐스트. 진입 전 전투 종료 여부를 먼저 확인
void ACombatManager::SetPhase(ETurnPhase NewPhase)
{
	CheckCombatEnd();

	CurrentPhase = NewPhase;
	OnPhaseChanged.Broadcast(NewPhase);

	switch (NewPhase)
	{
		case ETurnPhase::DrawPhase:            UE_LOG(LogTemp, Warning, TEXT("[Turn %d] 드로우턴"), TurnCount);      break;
		case ETurnPhase::PlayerActionPhase:    UE_LOG(LogTemp, Warning, TEXT("[Turn %d] 플레이어 행동턴"), TurnCount); break;
		case ETurnPhase::PlayerExecutionPhase: UE_LOG(LogTemp, Warning, TEXT("[Turn %d] 행동 큐 실행턴"), TurnCount); break;
		case ETurnPhase::EnemyPhase:           UE_LOG(LogTemp, Warning, TEXT("[Turn %d] 몬스터턴"), TurnCount);      break;
	}
}

// 지정 유닛 배열의 Shield를 리셋하고 버프/디버프 지속 시간을 차감.
// 플레이어는 DrawPhase에, 적은 EnemyPhase 시작 시 각각 호출한다
void ACombatManager::ApplyTurnStartEffects(const TArray<AUnit*>& Units)
{
	for (AUnit* Unit : Units)
	{
		if (!Unit) continue;
		UStatusEffectComponent* SEC = Unit->FindComponentByClass<UStatusEffectComponent>();
		if (!SEC) continue;

		// Shield 리셋 (매 턴 시작 시 0으로 초기화)
		if (SEC->GetEffectValue(EEffectType::Shield) > 0)
			SEC->SetEffectValue(EEffectType::Shield, 0);

		// 수치형 버프/디버프 틱 (Regen 회복, Burn 데미지, 시간제 스택 감소)
		UEffectManager::TickEffects(Unit);

		// 오브젝트형 효과 틱 (UStatusEffect 파생 클래스)
		for (UStatusEffect* Effect : SEC->Active)
			if (Effect) Effect->OnTurnEnd();
	}
}

// 턴 카운트를 올리고 DrawPhase → PlayerActionPhase 순서로 진행
void ACombatManager::StartTurn()
{
	TurnCount++;
	// 새 턴 시작 시 이전 턴 기록 초기화
	ActionQueue.Empty();
	UE_LOG(LogTemp, Warning, TEXT("[CombatManager] Turn %d 시작"), TurnCount);

	SetPhase(ETurnPhase::DrawPhase);
	ApplyTurnStartEffects(SpawnedPlayers); // DrawPhase: 플레이어 Shield 리셋 + 상태효과 tick
	PlanAllEnemyActions(); // DrawPhase: 모든 적의 이번 턴 행동을 미리 결정

	// 모든 플레이어 유닛의 CardUserComponent 에 턴 시작 드로우 요청
	for (AUnit* Player : SpawnedPlayers)
	{
		if (!Player) continue;
		UCardUserComponent* CardComp = Player->FindComponentByClass<UCardUserComponent>();
		if (CardComp)
			CardComp->DrawStartOfTurn();
	}

	SetPhase(ETurnPhase::PlayerActionPhase);
}

// PlayerActionPhase에서만 동작. 사용한 카드를 ActionQueue에 추가
void ACombatManager::QueuePlayerAction(const FCardDataRow& Card, int32 CasterIndex, FName CardRowName, AUnit* TargetOverride)
{
	if (CurrentPhase != ETurnPhase::PlayerActionPhase) return;

	FQueuedAction Action;
	Action.Card           = Card;
	Action.CasterIndex    = CasterIndex;
	Action.CardRowName    = CardRowName;
	Action.TargetOverride = TargetOverride;
	ActionQueue.Add(Action);

	UE_LOG(LogTemp, Warning, TEXT("[CombatManager] 큐 추가: %s Target=%s (큐 크기=%d)"),
		*Card.CardID.ToString(),
		TargetOverride ? *TargetOverride->GetName() : TEXT("default"),
		ActionQueue.Num());
}

// 플레이어 행동 입력을 종료하고 적 턴으로 직접 전환 (카드 효과는 이미 즉시 실행됨)
void ACombatManager::EndPlayerActionPhase()
{
	if (CurrentPhase != ETurnPhase::PlayerActionPhase) return;
	StartEnemyPhase();
}

// 적 턴 페이즈를 시작하고 첫 번째 적부터 행동을 진행
void ACombatManager::StartEnemyPhase()
{
	CurrentEnemyIndex = 0;
	SetPhase(ETurnPhase::EnemyPhase);
	ApplyTurnStartEffects(SpawnedEnemies); // EnemyPhase: 적 Shield 리셋 + 상태효과 tick
	ExecuteNextEnemyAction();
}

// 죽은 적을 건너뛰며 현재 인덱스의 적이 행동하도록 처리.
// 모든 적이 행동 완료되면 다음 턴을 시작
void ACombatManager::ExecuteNextEnemyAction()
{
	// 죽은 적 건너뜀
	while (SpawnedEnemies.IsValidIndex(CurrentEnemyIndex) &&
		   (!SpawnedEnemies[CurrentEnemyIndex] || !SpawnedEnemies[CurrentEnemyIndex]->IsAlive()))
	{
		CurrentEnemyIndex++;
	}

	if (!SpawnedEnemies.IsValidIndex(CurrentEnemyIndex))
	{
		// 모든 적 행동 완료 → 다음 턴
		StartTurn();
		return;
	}

	AUnit* Enemy = SpawnedEnemies[CurrentEnemyIndex];
	OnEnemyTurnStart.Broadcast(CurrentEnemyIndex);

	// PlanNextAction에서 정해둔 행동을 실행
	UNPCBrainComponent* Brain = Enemy->FindComponentByClass<UNPCBrainComponent>();
	if (Brain)
		ExecuteEnemyAction(Enemy, Brain->PendingAction);

	// EnemyActionDelay 후 다음 적 자동 진행 (에디터 Combat|Timing에서 조정)
	GetWorldTimerManager().SetTimer(
		EnemyTimerHandle,
		this, &ACombatManager::OnEnemyActionComplete,
		EnemyActionDelay, false);
}

// 현재 적의 행동이 끝났을 때 호출. 다음 적으로 인덱스를 넘김
void ACombatManager::OnEnemyActionComplete()
{
	CurrentEnemyIndex++;
	ExecuteNextEnemyAction();
}

// Index번 플레이어의 StatComponent를 반환. 유효하지 않으면 nullptr
UStatComponent* ACombatManager::GetPlayerStat(int32 Index) const
{
	if (!SpawnedPlayers.IsValidIndex(Index) || !SpawnedPlayers[Index]) return nullptr;
	return SpawnedPlayers[Index]->GetStat();
}

// Index번 적의 StatComponent를 반환. 유효하지 않으면 nullptr
UStatComponent* ACombatManager::GetEnemyStat(int32 Index) const
{
	if (!SpawnedEnemies.IsValidIndex(Index) || !SpawnedEnemies[Index]) return nullptr;
	return SpawnedEnemies[Index]->GetStat();
}

// DrawPhase에 살아있는 모든 적의 행동을 미리 결정
// 적 시점: Allies=SpawnedEnemies(아군), Enemies=SpawnedPlayers(적)
void ACombatManager::PlanAllEnemyActions()
{
	for (AUnit* Enemy : SpawnedEnemies)
	{
		if (!Enemy || !Enemy->IsAlive()) continue;
		UNPCBrainComponent* Brain = Enemy->FindComponentByClass<UNPCBrainComponent>();
		if (Brain)
			Brain->PlanNextAction(SpawnedEnemies, SpawnedPlayers);
	}
}

// 적 한 명의 FEnemyAction을 실제로 실행
// - Attack: IntentComponent에 저장된 타겟 or 광역 대상에게 Hits 횟수만큼 ProcessDamage
// - Defend: 자신에게 Shield 부여
// - Buff/Debuff: 미구현 (StatusEffect 오브젝트 연결 후 추가 예정)
// - NoAttack: 행동 없음
void ACombatManager::ExecuteEnemyAction(AUnit* Caster, const FEnemyAction& Action)
{
	if (!Caster || !Caster->IsAlive()) return;

	// 타겟 목록 결정 (적 시점에서 "Enemy" = 플레이어, "Ally" = 다른 적)
	TArray<AUnit*> Targets;
	switch (Action.TargetType)
	{
	case ETargetType::SingleEnemy:
	{
		// 1순위: IntentComponent에 PlanNextAction이 저장한 타겟
		AUnit* Resolved = nullptr;
		UIntentComponent* Intent = Caster->FindComponentByClass<UIntentComponent>();
		if (Intent && Intent->Current.Target.IsValid())
			Resolved = Intent->Current.Target.Get();

		// 2순위: IntentComponent 없거나 타겟 무효 → 살아있는 랜덤 플레이어 직접 선택
		if (!Resolved)
		{
			TArray<AUnit*> Alive;
			for (AUnit* P : SpawnedPlayers) { if (P && P->IsAlive()) Alive.Add(P); }
			if (!Alive.IsEmpty())
				Resolved = Alive[FMath::RandRange(0, Alive.Num() - 1)];
		}

		if (Resolved) Targets.Add(Resolved);
		break;
	}
	case ETargetType::SingleAlly:
	{
		// 1순위: IntentComponent 타겟
		AUnit* Resolved = nullptr;
		UIntentComponent* Intent = Caster->FindComponentByClass<UIntentComponent>();
		if (Intent && Intent->Current.Target.IsValid())
			Resolved = Intent->Current.Target.Get();

		// 2순위: 살아있는 랜덤 적 직접 선택
		if (!Resolved)
		{
			TArray<AUnit*> Alive;
			for (AUnit* E : SpawnedEnemies) { if (E && E->IsAlive() && E != Caster) Alive.Add(E); }
			if (!Alive.IsEmpty())
				Resolved = Alive[FMath::RandRange(0, Alive.Num() - 1)];
		}

		if (Resolved) Targets.Add(Resolved);
		break;
	}
	case ETargetType::AllEnemies: // 적 시점: 모든 플레이어
		for (AUnit* P : SpawnedPlayers) { if (P && P->IsAlive()) Targets.Add(P); }
		break;
	case ETargetType::AllAllies: // 적 시점: 모든 적
		for (AUnit* E : SpawnedEnemies) { if (E && E->IsAlive()) Targets.Add(E); }
		break;
	case ETargetType::Self:
		Targets.Add(Caster);
		break;
	default:
		break;
	}

	// 행동 종류별 실행
	switch (Action.IntentKind)
	{
	case EIntentKind::Attack:
		for (AUnit* Target : Targets)
		{
			// Hits 횟수만큼 반복 공격
			for (int32 i = 0; i < FMath::Max(1, Action.Hits); i++)
				UEffectManager::ProcessDamage(Target, Action.Value, Caster);
		}
		UE_LOG(LogTemp, Warning, TEXT("[CombatManager] %s 공격 → %d 타겟, %d 데미지 x%d"),
			*Caster->GetName(), Targets.Num(), Action.Value, Action.Hits);
		break;

	case EIntentKind::Defend:
		// Targets 배열 기준으로 Shield 부여 (Self → 자신, AllAllies → 아군 전체 등)
		// Targets가 비어있으면 자신에게 폴백
		if (Targets.IsEmpty()) Targets.Add(Caster);
		for (AUnit* Target : Targets)
			UEffectManager::ApplyEffect(Target, EEffectType::Shield, Action.Value);
		UE_LOG(LogTemp, Warning, TEXT("[CombatManager] %s 방어 → %d 타겟에 Shield +%d"),
			*Caster->GetName(), Targets.Num(), Action.Value);
		break;

	case EIntentKind::Buff:
	case EIntentKind::Debuff:
		// TODO: StatusEffect 오브젝트형 연결 시 구현
		UE_LOG(LogTemp, Warning, TEXT("[CombatManager] %s Buff/Debuff — 미구현"),
			*Caster->GetName());
		break;

	case EIntentKind::NoAttack:
	default:
		UE_LOG(LogTemp, Warning, TEXT("[CombatManager] %s 행동 없음"), *Caster->GetName());
		break;
	}
}
