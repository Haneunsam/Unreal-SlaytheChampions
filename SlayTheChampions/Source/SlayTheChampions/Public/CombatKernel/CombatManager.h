#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatManager.generated.h"

class UCombatStatComponent;
class ACombatantActor;
class UBoxComponent;

USTRUCT(BlueprintType)
struct FCombatantInitData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 MaxHP = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 Defence = 0;
};

UCLASS()
class SLAYTHECHAMPIONS_API ACombatManager : public AActor
{
	GENERATED_BODY()

public:
	ACombatManager();

	// ── 스폰 클래스 ───────────────────────────────────────────────
	UPROPERTY(EditAnywhere, Category = "Combat|Setup")
	TSubclassOf<ACombatantActor> PlayerClass;

	UPROPERTY(EditAnywhere, Category = "Combat|Setup")
	TSubclassOf<ACombatantActor> EnemyClass;

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

	// ── 슬롯 위치 마커 (BP 컴포넌트 패널에서 이동 가능) ───────────
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Slots")
	USceneComponent* PlayerSlot_0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Slots")
	USceneComponent* PlayerSlot_1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Slots")
	USceneComponent* EnemySlot_0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Slots")
	USceneComponent* EnemySlot_1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Slots")
	USceneComponent* EnemySlot_2;

	// ── 에디터 전용 시각화 박스 (인게임 숨김) ─────────────────────
	UPROPERTY(VisibleAnywhere, Category = "Combat|Slots")
	UBoxComponent* PlayerBox_0;

	UPROPERTY(VisibleAnywhere, Category = "Combat|Slots")
	UBoxComponent* PlayerBox_1;

	UPROPERTY(VisibleAnywhere, Category = "Combat|Slots")
	UBoxComponent* EnemyBox_0;

	UPROPERTY(VisibleAnywhere, Category = "Combat|Slots")
	UBoxComponent* EnemyBox_1;

	UPROPERTY(VisibleAnywhere, Category = "Combat|Slots")
	UBoxComponent* EnemyBox_2;

	// ── 런타임 조회 ───────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void InitCombat();

	UFUNCTION(BlueprintPure, Category = "Combat")
	UCombatStatComponent* GetPlayerStat(int32 Index) const;

	UFUNCTION(BlueprintPure, Category = "Combat")
	UCombatStatComponent* GetEnemyStat(int32 Index) const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TArray<ACombatantActor*> SpawnedPlayers;

	UPROPERTY()
	TArray<ACombatantActor*> SpawnedEnemies;

	void SetupSlot(USceneComponent*& OutSlot, UBoxComponent*& OutBox,
	               const FName& SlotName, const FName& BoxName,
	               const FVector& RelativeLocation, const FColor& Color);

	ACombatantActor* SpawnCombatant(TSubclassOf<ACombatantActor> ActorClass,
	                                USceneComponent* Slot,
	                                const FCombatantInitData& Data);
};
