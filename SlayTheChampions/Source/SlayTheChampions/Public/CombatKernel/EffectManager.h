#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Unit/StatusEffectComponent.h"
#include "EffectManager.generated.h"

class AUnit;

UCLASS()
class SLAYTHECHAMPIONS_API UEffectManager : public UObject
{
	GENERATED_BODY()

public:
	// 효과 부여 (Block 등 수치형)
	UFUNCTION(BlueprintCallable, Category = "Effect")
	static void ApplyEffect(AUnit* Target, EEffectType Type, int32 Value);

	// 데미지 파이프라인 (Block 흡수 후 TakeDamage 호출)
	static void ProcessDamage(AUnit* Target, int32 Damage, AUnit* Attacker);
};
