#include "Map/Area.h"

UArea::UArea()
{
}

void UArea::InitArea(const FAreaInfo& InInfo)
{
    AreaInfo = InInfo;
}

void UArea::DebugShowInfo()
{
    UE_LOG(LogTemp, Warning, TEXT("=== Area Created Debug ==="));
    UE_LOG(LogTemp, Warning, TEXT("Type: %d (0:Nomal, 1:Elite...)"), (int32)AreaInfo.AreaType);
    UE_LOG(LogTemp, Warning, TEXT("State: %d (0:Reday 1:Start 2:Running 3:End)"), (int32)AreaInfo.AreaState);
    UE_LOG(LogTemp, Warning, TEXT("Position: %s"), *AreaInfo.AreaPos.ToString());
    for (UArea* NextArea : NextAreas)
    {
        if (NextArea) 
        {
            UE_LOG(LogTemp, Warning, TEXT("NextRoomPos: %s"), *NextArea->AreaInfo.AreaPos.ToString());
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("=========================="));
}
