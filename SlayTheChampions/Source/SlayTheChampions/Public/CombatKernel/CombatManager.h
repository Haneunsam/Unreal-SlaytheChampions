#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CardDataTypes.h"
#include "CombatManager.generated.h"

class UStatComponent;
class AUnit;
class UBoxComponent;

// ── 턴 페이즈 ─────────────────────────────────────────────────────────
UENUM(BlueprintType)
enum class ETurnPhase : uint8
{
	DrawPhase             UMETA(DisplayName = "Draw Phase"),
	PlayerActionPhase     UMETA(DisplayName = "Player Action Phase"),
	PlayerExecutionPhase  UMETA(DisplayName = "Player Execution Phase"),
	EnemyPhase            UMETA(DisplayName = "Enemy Phase"),
};

// ── 플레이어 액션 큐 항목 ──────────────────────────────────────────────
USTRUCT(BlueprintType)
struct FQueuedAction
{
	GENERATED_BODY()

	UPROPERTY()
	FCardDataRow Card;

	UPROPERTY()
	int32 CasterIndex = 0;
};

// ── 스폰 초기 데이터 ───────────────────────────────────────────────────
USTRUCT(BlueprintType)
struct FCombatantInitData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 MaxHP = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 Defence = 0;
};

// ── 델리게이트 ─────────────────────────────────────────────────────────
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged,      ETurnPhase,    NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionExecuted,    FCardDataRow,  Card);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExecutionFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyTurnStart,    int32,         EnemyIndex);

UCLASS()
class SLAYTHECHAMPIONS_API ACombatManager : public AActor
{
	GENERATED_BODY()

public:
	ACombatManager();

	// ── 스폰 클래스 ───────────────────────────────────────────────
	UPROPERTY(EditAnywhere, Category = "Combat|Setup")
	TSubclassOf<AUnit> PlayerClass;

	UPROPERTY(EditAnywhere, Category = "Combat|Setup")
	TSubclassOf<AUnit> EnemyClass;

	// ── 스폰 수 ──────────────────────────────────────────────────
	UPROPERTY(EditAnywhere, Category = "Combat|Setup", meta = (ClampMin = "1", ClampMax = "2"))
	int32 PlayerCount = 1;

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
	UPROPERTY(BlueprintReadOnly, Category = "Turn")
	ETurnPhase CurrentPhase = ETurnPhase::DrawPhase;

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
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void InitCombat();

	// ── 카드 효과 실행 ────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ExecuteCard(const FCardDataRow& Card, int32 CasterIndex);

	// ── 턴 함수 ───────────────────────────────────────────────────
	// DrawPhase 시작 (버프/디버프 처리 → 방어도 리셋 → PlayerActionPhase)
	UFUNCTION(BlueprintCallable, Category = "Turn")
	void StartTurn();

	// PlayerActionPhase: 카드 큐에 추가
	UFUNCTION(BlueprintCallable, Category = "Turn")
	void QueuePlayerAction(const FCardDataRow& Card, int32 CasterIndex);

	// PlayerActionPhase 종료 → PlayerExecutionPhase 진입
	UFUNCTION(BlueprintCallable, Category = "Turn")
	void EndPlayerActionPhase();

	// 큐에서 하나씩 실행 (애니메이션 완료 후 Blueprint에서 재호출)
	UFUNCTION(BlueprintCallable, Category = "Turn")
	void ExecuteNextAction();

	// 큐 전부 즉시 실행 (스킵)
	UFUNCTION(BlueprintCallable, Category = "Turn")
	void SkipToEnd();

	// 적 행동 완료 후 Blueprint에서 호출 → 다음 적으로 진행
	UFUNCTION(BlueprintCallable, Category = "Turn")
	void OnEnemyActionComplete();

	// ── 스탯 조회 ─────────────────────────────────────────────────
	UFUNCTION(BlueprintPure, Category = "Combat")
	UStatComponent* GetPlayerStat(int32 Index) const;

	UFUNCTION(BlueprintPure, Category = "Combat")
	UStatComponent* GetEnemyStat(int32 Index) const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TArray<AUnit*> SpawnedPlayers;

	UPROPERTY()
	TArray<AUnit*> SpawnedEnemies;

	UPROPERTY()
	TArray<FQueuedAction> ActionQueue;

	int32 CurrentEnemyIndex = 0;

	void SetPhase(ETurnPhase NewPhase);
	void CheckCombatEnd();
	void ApplyTurnStartEffects();
	void StartEnemyPhase();
	void ExecuteNextEnemyAction();

	UBoxComponent* SetupBox(const FName& BoxName, const FVector& RelativeLocation, const FColor& Color);
	AUnit* SpawnCombatant(TSubclassOf<AUnit> ActorClass, UBoxComponent* Box, const FCombatantInitData& Data);
};
