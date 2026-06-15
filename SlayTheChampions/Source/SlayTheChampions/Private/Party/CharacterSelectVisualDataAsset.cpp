#include "Party/CharacterSelectVisualDataAsset.h"

bool UCharacterSelectVisualDataAsset::FindVisualData(FName UnitID, FCharacterSelectVisualData& OutVisualData) const
{
	const FCharacterSelectVisualData* FoundVisualData = Visuals.FindByPredicate(
		[UnitID](const FCharacterSelectVisualData& VisualData)
		{
			return VisualData.UnitID == UnitID;
		});

	if (!FoundVisualData)
	{
		return false;
	}

	OutVisualData = *FoundVisualData;
	return true;
}
