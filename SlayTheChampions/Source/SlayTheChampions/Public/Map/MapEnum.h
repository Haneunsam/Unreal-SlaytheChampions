#pragma once

#include "CoreMinimal.h"
#include "MapEnum.generated.h"

/*Area 상태*/
UENUM(BlueprintType)
enum class EAreaState : uint8
{
    Ready = 0   UMETA(DisplayName = "Ready"),
    Start       UMETA(DisplayName = "Start"),
    Running     UMETA(DisplayName = "Running"),
    End         UMETA(DisplayName = "End")
};

/*Area 타입*/
UENUM(BlueprintType)
enum class EAreaType : uint8
{
    Normal = 0      UMETA(DisplayName = "Normal"),
    Elite           UMETA(DisplayName = "Elite"),
    Boss            UMETA(DisplayName = "Boss"),
    Event           UMETA(DisplayName = "Event"),
    Rest            UMETA(DisplayName = "Rest"),
    Shop            UMETA(DisplayName = "Shop"),
    Reword          UMETA(DisplayName = "Reword"),
    ArtifactEvent   UMETA(DisplayName = "ArtifactEvent"),
    None = 99       UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class EAreaVisitState : uint8
{
    Locked = 0  UMETA(DisplayName = "Locked"),
    Visited     UMETA(DisplayName = "Visited"),
    Cleared     UMETA(DisplayName = "Cleared"),
    None = 99   UMETA(DisplayName = "None"),
};


