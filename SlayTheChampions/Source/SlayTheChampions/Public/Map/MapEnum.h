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

/*Area 이동가능 여부*/
UENUM(BlueprintType)
enum class EAreaVisitState : uint8
{
    Locked = 0  UMETA(DisplayName = "Locked"),
    Visited     UMETA(DisplayName = "Visited"),
    Cleared     UMETA(DisplayName = "Cleared"),
    None = 99   UMETA(DisplayName = "None"),
};

/*Run 진행 상태*/
UENUM(BlueprintType)
enum class ERunState : uint8
{
    Ready = 0       UMETA(DisplayName = "Ready"),
    RunInit         UMETA(DisplayName = "RunInit"),
    StageSelect     UMETA(DisplayName = "StageSelect"),
    RoomEntered     UMETA(DisplayName = "RoomEntered"),
    StageClear      UMETA(DisplayName = "StageClear"),
    RunClear        UMETA(DisplayName = "RunClear"),
    RunFail         UMETA(DisplayName = "RunFail"),
    None = 99       UMETA(DisplayName = "None"),
};


