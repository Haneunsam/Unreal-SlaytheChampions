#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EncounterData.generated.h"

/**
 * UEncounterData
 * 한 전투의 적 구성 = EnemyTable(도감)에서 뽑아올 EnemyID 목록.
 * CombatManager의 Encounter 슬롯에 지정하면 EnemyTable 경로로 스폰된다.
 * EnemyIDs 순서대로 EnemyBox_0/1/2 위치에 최대 3마리 배치.
 *
 * 역할 분담:
 *   EnemyTable(도감) — 적별 풀스펙(HP·패턴·기믹·메시·애님)을 EnemyID로 보관
 *   EncounterData(이 에셋) — 이번 전투에 어떤 EnemyID가 나오는지만 정의
 *   BP_Enemy(껍데기) — 데이터를 주입받는 제네릭 액터 1종
 */
UCLASS(BlueprintType)
class SLAYTHECHAMPIONS_API UEncounterData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// EnemyTable에서 이번 전투에 등장시킬 EnemyID 목록 (최대 3, 스폰 박스 순서대로)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Encounter")
	TArray<FName> EnemyIDs;
};
