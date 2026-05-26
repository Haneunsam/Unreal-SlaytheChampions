#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Unit/StatusEffectComponent.h"
#include "EffectManager.generated.h"

class AUnit;

/**
 * UEffectManager
 * 전투 중 효과 부여 및 데미지 처리를 담당하는 정적 유틸 클래스.
 * Shield 흡수 → HP 감소 순서의 데미지 파이프라인을 중앙에서 관리한다.
 */
UCLASS()
class SLAYTHECHAMPIONS_API UEffectManager : public UObject
{
	GENERATED_BODY()

public:
	// 수치형 효과(Shield 등)를 대상에게 누적 부여
	UFUNCTION(BlueprintCallable, Category = "Effect")
	static void ApplyEffect(AUnit* Target, EEffectType Type, int32 Value);

	// 데미지 파이프라인: Shield로 먼저 흡수하고 남은 수치를 HP에 적용
	static void ProcessDamage(AUnit* Target, int32 Damage, AUnit* Attacker);
};
