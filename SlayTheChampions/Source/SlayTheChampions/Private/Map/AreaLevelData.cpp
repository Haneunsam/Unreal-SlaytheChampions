#include "Map/AreaLevelData.h"

FName UAreaLevelData::GetLevelName(EAreaType AreaType) const
{
	switch (AreaType)
	{
	case EAreaType::Normal: return NormalLevelName;
	case EAreaType::Elite: return EliteLevelName;
	case EAreaType::Boss: return BossLevelName;
	case EAreaType::Event: return EventLevelName;
	case EAreaType::Rest: return RestLevelName;
	case EAreaType::Shop: return ShopLevelName;
	case EAreaType::Reword: return RewardLevelName;
	case EAreaType::ArtifactEvent: return ArtifactEventLevelName;
	default: return NAME_None;
	}
}

