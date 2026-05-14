#include "Map/Area.h"

UArea::UArea()
{
}

void UArea::BeginPlay()
{
    Super::BeginPlay();
}

void UArea::InitArea(const FAreaInfo& InInfo)
{
    AreaInfo = InInfo;
}
