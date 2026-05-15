#pragma once

#include "CoreMinimal.h"
#include "AttackTypeEnum.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	Attack  UMETA(DisplayName = "attack"),
	Defence UMETA(DisplayName = "defence")
};
