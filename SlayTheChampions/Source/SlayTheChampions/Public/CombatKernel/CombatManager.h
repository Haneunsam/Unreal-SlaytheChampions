#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CardDataTypes.h"
#include "CombatManager.generated.h"

class UStatComponent;
class AUnit;
class UBoxComponent;

USTRUCT(BlueprintType)
struct FCombatantInitData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 MaxHP = 100;

	// [임시] UStatComponent에 방어도가 생기면 주입 로직 추가 후 이 주석 제거
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

	// ── 스폰 위치 박스 (에디터에서 직접 이동하여 위치 조정) ─────────
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

	// ── 런타임 조회 ───────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void InitCombat();

	UFUNCTION(BlueprintPure, Category = "Combat")
	UStatComponent* GetPlayerStat(int32 Index) const;

	UFUNCTION(BlueprintPure, Category = "Combat")
	UStatComponent* GetEnemyStat(int32 Index) const;

	// 카드 효과 실행 (CasterIndex: 시전자 플레이어 인덱스)
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ExecuteCard(const FCardDataRow& Card, int32 CasterIndex);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TArray<AUnit*> SpawnedPlayers;

	UPROPERTY()
	TArray<AUnit*> SpawnedEnemies;

	// 박스 컴포넌트 생성 및 루트에 부착
	UBoxComponent* SetupBox(const FName& BoxName,
	                        const FVector& RelativeLocation,
	                        const FColor& Color);

	// 박스 위치를 기준으로 유닛 스폰
	AUnit* SpawnCombatant(TSubclassOf<AUnit> ActorClass,
	                      UBoxComponent* Box,
	                      const FCombatantInitData& Data);
};
