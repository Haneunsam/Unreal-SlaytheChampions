#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Card/CardDataTypes.h"
#include "Unit/CombatTypes.h"
#include "CombatManager.generated.h"

class UStatComponent;
class AUnit;
class UBoxComponent;

/**
 * ETurnPhase
 * 한 턴의 진행 순서. DrawPhase → PlayerActionPhase → PlayerExecutionPhase → EnemyPhase.
 * SetPhase()로 전환되며, 각 페이즈 진입 시 CheckCombatEnd()가 먼저 실행된다.
 */
UENUM(BlueprintType)
enum class ETurnPhase : uint8
{
	DrawPhase             UMETA(DisplayName = "Draw Phase"),
	PlayerActionPhase     UMETA(DisplayName = "Player Action Phase"),
	PlayerExecutionPhase  UMETA(DisplayName = "Player Execution Phase"),
	EnemyPhase            UMETA(DisplayName = "Enemy Phase"),
};

/**
 * FQueuedAction
 * PlayerActionPhase에서 큐에 쌓이는 카드 실행 요청 하나.
 * ExecuteNextAction()이 순서대로 꺼내 처리한다.
 */
USTRUCT(BlueprintType)
struct FQueuedAction
{
	GENERATED_BODY()

	// 실행할 카드 데이터
	UPROPERTY()
	FCardDataRow Card;

	// 카드를 사용한 플레이어 인덱스 (SpawnedPlayers 기준)
	UPROPERTY()
	int32 CasterIndex = 0;
};

/**
 * FCombatantInitData
 * InitCombat에서 유닛 스폰 시 주입하는 초기 스탯.
 * 에디터 Details 패널에서 슬롯별로 설정한다.
 */
USTRUCT(BlueprintType)
struct FCombatantInitData
{
	GENERATED_BODY()

	// 최대 HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 MaxHP = 100;

	// 초기 방어도 ([임시] StatComponent에 방어도 추가 후 실제 주입 예정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 Defence = 0;
};

// ── 델리게이트 ─────────────────────────────────────────────────────────
// 페이즈 전환 시 브로드캐스트 (UI 갱신, 코스트 초기화 등에 바인딩)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged,      ETurnPhase,    NewPhase);
// 카드 하나가 실행될 때마다 브로드캐스트 (애니메이션 트리거용)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionExecuted,    FCardDataRow,  Card);
// PlayerExecutionPhase에서 ActionQueue가 전부 소진됐을 때 브로드캐스트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExecutionFinished);
// EnemyPhase에서 특정 인덱스의 적이 행동을 시작할 때 브로드캐스트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyTurnStart,    int32,         EnemyIndex);

/**
 * ACombatManager
 * 전투 전체를 관리하는 중앙 Actor.
 * 유닛 스폰, 턴 페이즈 전환, 카드 실행, 적 AI 진행 순서를 담당한다.
 * 레벨에 하나만 배치하고 에디터 Details에서 슬롯별 데이터를 설정한다.
 */
UCLASS()
class SLAYTHECHAMPIONS_API ACombatManager : public AActor
{
	GENERATED_BODY()

public:
	ACombatManager();

	// ── 스폰 클래스 ───────────────────────────────────────────────
	// 스폰할 플레이어 유닛 Blueprint 클래스
	UPROPERTY(EditAnywhere, Category = "Combat|Setup")
	TSubclassOf<AUnit> PlayerClass;

	// 스폰할 적 유닛 Blueprint 클래스
	UPROPERTY(EditAnywhere, Category = "Combat|Setup")
	TSubclassOf<AUnit> EnemyClass;

	// ── 스폰 수 ──────────────────────────────────────────────────
	// 스폰할 플레이어 수 (1~2)
	UPROPERTY(EditAnywhere, Category = "Combat|Setup", meta = (ClampMin = "1", ClampMax = "2"))
	int32 PlayerCount = 1;

	// 스폰할 적 수 (1~3)
	UPROPERTY(EditAnywhere, Category = "Combat|Setup", meta = (ClampMin = "1", ClampMax = "3"))
	int32 EnemyCount = 1;

	// ── 플레이어 슬롯 데이터 ──────────────────────────────────────
	UPROPERTY(EditAnywhere, Category = "Combat|PlayerData")
	FCombatantInitData PlayerData_0;

	UPROPERTY(EditAnywhere, Category = "Combat|PlayerData")
	FCombatantInitData PlayerData_1;

	// ── 적 슬롯 데이터 ────────────────────────────────────────────
	UPROPERTY(EditAnywhere, Category = "Combat|EnemyData")
	FCombatantInitData EnemyData_0;

	UPROPERTY(EditAnywhere, Category = "Combat|EnemyData")
	FCombatantInitData EnemyData_1;

	UPROPERTY(EditAnywhere, Category = "Combat|EnemyData")
	FCombatantInitData EnemyData_2;

	// ── 스폰 위치 박스 ────────────────────────────────────────────
	// 에디터에서 직접 이동하여 스폰 위치를 조정하는 BoxComponent 슬롯들
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Slots")
	UBoxComponent* PlayerBox_0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Slots")
	UBoxComponent* PlayerBox_1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Slots")
	UBoxComponent* EnemyBox_0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Slots")
	UBoxComponent* EnemyBox_1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Slots")
	UBoxComponent* EnemyBox_2;

	// ── 턴 상태 ───────────────────────────────────────────────────
	// 현재 턴 페이즈 (읽기 전용 — SetPhase()로만 변경)
	UPROPERTY(BlueprintReadOnly, Category = "Turn")
	ETurnPhase CurrentPhase = ETurnPhase::DrawPhase;

	// 현재 턴 번호 (1부터 시작)
	UPROPERTY(BlueprintReadOnly, Category = "Turn")
	int32 TurnCount = 0;

	// ── 턴 델리게이트 ─────────────────────────────────────────────
	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FOnPhaseChanged OnPhaseChanged;

	// 플레이어 액션 하나 실행됐을 때 (애니메이션 트리거용)
	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FOnActionExecuted OnActionExecuted;

	// 플레이어 큐 전부 소진됐을 때
	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FOnExecutionFinished OnExecutionFinished;

	// 적 행동 시작 (EnemyIndex번 적 행동할 차례)
	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FOnEnemyTurnStart OnEnemyTurnStart;

	// ── 전투 초기화 ───────────────────────────────────────────────
	// 유닛을 스폰하고 1턴을 시작. BeginPlay에서 자동 호출
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void InitCombat();

	// ── 카드 효과 실행 ────────────────────────────────────────────
	// 카드 데이터로 타겟을 결정하고 데미지·회복·Shield 효과를 적용
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ExecuteCard(const FCardDataRow& Card, int32 CasterIndex);

	// ── 턴 함수 ───────────────────────────────────────────────────
	// DrawPhase 시작: Shield 리셋 → 버프/디버프 tick → PlayerActionPhase
	UFUNCTION(BlueprintCallable, Category = "Turn")
	void StartTurn();

	// PlayerActionPhase: 사용한 카드를 ActionQueue에 추가
	UFUNCTION(BlueprintCallable, Category = "Turn")
	void QueuePlayerAction(const FCardDataRow& Card, int32 CasterIndex);

	// PlayerActionPhase 종료 → PlayerExecutionPhase 진입 후 큐 실행 시작
	UFUNCTION(BlueprintCallable, Category = "Turn")
	void EndPlayerActionPhase();

	// ActionQueue에서 카드를 하나 꺼내 실행. 애니메이션 완료 후 Blueprint에서 재호출
	UFUNCTION(BlueprintCallable, Category = "Turn")
	void ExecuteNextAction();

	// ActionQueue를 즉시 전부 실행하고 적 턴으로 전환 (애니메이션 스킵)
	UFUNCTION(BlueprintCallable, Category = "Turn")
	void SkipToEnd();

	// 적 행동 완료 후 호출 → 다음 적으로 인덱스 전진
	UFUNCTION(BlueprintCallable, Category = "Turn")
	void OnEnemyActionComplete();

	// ── 스탯 조회 ─────────────────────────────────────────────────
	// Index번 플레이어의 StatComponent 반환 (없으면 nullptr)
	UFUNCTION(BlueprintPure, Category = "Combat")
	UStatComponent* GetPlayerStat(int32 Index) const;

	// Index번 적의 StatComponent 반환 (없으면 nullptr)
	UFUNCTION(BlueprintPure, Category = "Combat")
	UStatComponent* GetEnemyStat(int32 Index) const;

	// 스폰된 플레이어 유닛 목록 반환 (BattleMainWidget 등 외부 시스템에서 참조용)
	UFUNCTION(BlueprintPure, Category = "Combat")
	const TArray<AUnit*>& GetSpawnedPlayers() const { return SpawnedPlayers; }

protected:
	virtual void BeginPlay() override;

private:
	// 스폰된 플레이어 유닛 목록
	UPROPERTY()
	TArray<AUnit*> SpawnedPlayers;

	// 스폰된 적 유닛 목록
	UPROPERTY()
	TArray<AUnit*> SpawnedEnemies;

	// 플레이어가 이번 턴에 사용한 카드 실행 대기열
	UPROPERTY()
	TArray<FQueuedAction> ActionQueue;

	// EnemyPhase에서 현재 행동 중인 적 인덱스
	int32 CurrentEnemyIndex = 0;

	// 페이즈를 전환하고 CheckCombatEnd → OnPhaseChanged 브로드캐스트
	void SetPhase(ETurnPhase NewPhase);
	// 전투 종료 조건(전멸) 확인 후 로그 출력 (TODO: 화면 전환 연결)
	void CheckCombatEnd();
	// 지정 유닛의 Shield를 리셋하고 상태효과 tick. 플레이어·적을 분리 호출한다
	void ApplyTurnStartEffects(const TArray<AUnit*>& Units);
	// EnemyPhase를 시작하고 첫 번째 살아있는 적부터 행동을 진행
	void StartEnemyPhase();
	// 죽은 적을 건너뛰며 CurrentEnemyIndex 적이 행동하도록 처리
	void ExecuteNextEnemyAction();

	// DrawPhase에 살아있는 모든 적의 NPCBrainComponent::PlanNextAction을 호출
	void PlanAllEnemyActions();
	// 적 한 명의 FEnemyAction을 실행 (Attack → ProcessDamage, Defend → Shield 부여)
	void ExecuteEnemyAction(AUnit* Caster, const FEnemyAction& Action);

	// 스폰 위치 BoxComponent를 생성하고 루트에 부착하는 헬퍼
	UBoxComponent* SetupBox(const FName& BoxName, const FVector& RelativeLocation, const FColor& Color);
	// 지정 클래스의 유닛을 Box 위치에 스폰하고 초기 스탯·위젯 연결
	AUnit* SpawnCombatant(TSubclassOf<AUnit> ActorClass, UBoxComponent* Box, const FCombatantInitData& Data);
};
