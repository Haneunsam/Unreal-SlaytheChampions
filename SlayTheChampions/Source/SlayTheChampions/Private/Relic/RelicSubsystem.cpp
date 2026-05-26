#include "Relic/RelicSubsystem.h"

#include "Engine/DataTable.h"
#include "UObject/EnumProperty.h"

void URelicSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	static const FSoftObjectPath RelicTablePath(TEXT("/Game/04_Data/RelicDataRow.RelicDataRow"));
	static const FSoftObjectPath RelicEffectTablePath(TEXT("/Game/04_Data/RelicEffectRow.RelicEffectRow"));

	if (UDataTable* LoadedRelicTable = Cast<UDataTable>(RelicTablePath.TryLoad()))
	{
		RelicInfoTable = LoadedRelicTable;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[RelicSubsystem] Failed to load relic data table: %s"), *RelicTablePath.ToString());
	}

	if (UDataTable* LoadedRelicEffectTable = Cast<UDataTable>(RelicEffectTablePath.TryLoad()))
	{
		RelicEffectsTable = LoadedRelicEffectTable;
	}

	RebuildRelicCache();
}

FRelicEffectData URelicSubsystem::MakeRelicEffectData(const FRelicEffectRow& EffectRow)
{
	FRelicEffectData EffectData;
	EffectData.Order = EffectRow.Order;
	EffectData.BuffType = EffectRow.BuffType;
	EffectData.Value = EffectRow.Value;
	EffectData.EffectiveDate = EffectRow.EffectiveDate;
	EffectData.TriggerCondition = EffectRow.TriggerCondition;
	EffectData.TriggerValue = EffectRow.TriggerValue;
	EffectData.TriggerUsageType = EffectRow.TriggerUsageType;
	return EffectData;
}

void URelicSubsystem::FillRelicRuntimeData(const FRelicDataRow& RelicRow, TArray<FRelicEffectData>&& Effects, FRelicRuntimeData& OutRelicData)
{
	OutRelicData.RelicID = RelicRow.RelicID;
	OutRelicData.RelicName = RelicRow.RelicName;
	OutRelicData.Description = RelicRow.Description;
	OutRelicData.Rarity = RelicRow.Rarity;
	OutRelicData.RelicSourceType = RelicRow.RelicSourceType;
	OutRelicData.TargetScope = RelicRow.TargetScope;
	OutRelicData.EffectiveDate = RelicRow.EffectiveDate;
	OutRelicData.Effects = MoveTemp(Effects);
}

bool URelicSubsystem::GetCachedRelicData(FName InRelicID, FRelicRuntimeData& OutRelicData) const
{
	if (const FRelicRuntimeData* FoundRelic = Map_Relics.Find(InRelicID))
	{
		OutRelicData = *FoundRelic;
		return true;
	}

	return false;
}

TArray<FRelicRuntimeData> URelicSubsystem::GetCachedRelics() const
{
	return Relics;
}

void URelicSubsystem::RebuildRelicCache()
{
	Relics.Reset();
	Map_Relics.Reset();

	if (!RelicInfoTable || !RelicEffectsTable)
	{
		return;
	}

	for (const FName& RowName : RelicInfoTable->GetRowNames())
	{
		const FRelicDataRow* RelicRow = RelicInfoTable->FindRow<FRelicDataRow>(RowName, TEXT("URelicSubsystem::RebuildRelicCache"));
		if (!RelicRow)
		{
			continue;
		}

		TArray<FRelicEffectData> Effects;
		for (const FName& EffectRowName : RelicEffectsTable->GetRowNames())
		{
			const FRelicEffectRow* EffectRow = RelicEffectsTable->FindRow<FRelicEffectRow>(EffectRowName, TEXT("URelicSubsystem::RebuildRelicCache"));
			if (!EffectRow || EffectRow->RelicID != RelicRow->RelicID)
			{
				continue;
			}

			Effects.Add(MakeRelicEffectData(*EffectRow));
		}

		Effects.Sort([](const FRelicEffectData& A, const FRelicEffectData& B)
		{
			return A.Order < B.Order;
		});

		FRelicRuntimeData RuntimeData;
		FillRelicRuntimeData(*RelicRow, MoveTemp(Effects), RuntimeData);
		Map_Relics.Add(RelicRow->RelicID, RuntimeData);
		Relics.Add(RuntimeData);
	}
}

