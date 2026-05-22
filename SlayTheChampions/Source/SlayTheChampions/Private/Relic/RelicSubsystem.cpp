#include "Relic/RelicSubsystem.h"

#include "Engine/DataTable.h"

void URelicSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	static const FSoftObjectPath RelicTablePath(TEXT("/Game/04_Data/T_RelicData.T_RelicData"));
	static const FSoftObjectPath RelicEffectTablePath(TEXT("/Game/04_Data/T_RelicEffectData.T_RelicEffectData"));

	if (UDataTable* LoadedRelicTable = Cast<UDataTable>(RelicTablePath.TryLoad()))
	{
		LoadRelicDataTable(LoadedRelicTable);
	}

	if (UDataTable* LoadedRelicEffectTable = Cast<UDataTable>(RelicEffectTablePath.TryLoad()))
	{
		LoadRelicEffectDataTable(LoadedRelicEffectTable);
	}
}

void URelicSubsystem::LoadRelicDataTable(UDataTable* InTable)
{
	RelicDataTable = InTable;
}

void URelicSubsystem::LoadRelicEffectDataTable(UDataTable* InTable)
{
	RelicEffectDataTable = InTable;
}

const FRelicDataRow* URelicSubsystem::GetRelicDataRow(FName InRelicID) const
{
	if (!RelicDataTable)
	{
		return nullptr;
	}

	for (const FName& RowName : RelicDataTable->GetRowNames())
	{
		const FRelicDataRow* Row = RelicDataTable->FindRow<FRelicDataRow>(RowName, TEXT("URelicSubsystem::GetRelicDataRow"));

		if (Row && Row->RelicID == InRelicID)
		{
			return Row;
		}
	}

	return nullptr;
}

TArray<FRelicEffectData> URelicSubsystem::GetRelicEffectData(FName InRelicID) const
{
	TArray<FRelicEffectData> Result;

	if (!RelicEffectDataTable)
	{
		return Result;
	}

	for (const FName& RowName : RelicEffectDataTable->GetRowNames())
	{
		const FRelicEffectRow* Row = RelicEffectDataTable->FindRow<FRelicEffectRow>(RowName, TEXT("URelicSubsystem::GetRelicEffectData"));

		if (!Row || Row->RelicID != InRelicID)
		{
			continue;
		}

		FRelicEffectData EffectData;
		EffectData.Order = Row->Order;
		EffectData.BuffType = Row->BuffType;
		EffectData.Value = Row->Value;
		EffectData.EffectiveDate = Row->EffectiveDate;
		EffectData.TriggerCondition = Row->TriggerCondition;
		EffectData.TriggerValue = Row->TriggerValue;
		EffectData.TriggerUsageType = Row->TriggerUsageType;
		Result.Add(EffectData);
	}

	Result.Sort([](const FRelicEffectData& A, const FRelicEffectData& B)
	{
		return A.Order < B.Order;
	});

	return Result;
}

bool URelicSubsystem::GetRelicRuntimeData(FName InRelicID, FRelicRuntimeData& OutRelicData) const
{
	const FRelicDataRow* RelicRow = GetRelicDataRow(InRelicID);

	if (!RelicRow)
	{
		return false;
	}

	OutRelicData.RelicID = RelicRow->RelicID;
	OutRelicData.RelicName = RelicRow->RelicName;
	OutRelicData.Description = RelicRow->Description;
	OutRelicData.Rarity = RelicRow->Rarity;
	OutRelicData.RelicSourceType = RelicRow->RelicSourceType;
	OutRelicData.TargetScope = RelicRow->TargetScope;
	OutRelicData.EffectiveDate = RelicRow->EffectiveDate;
	OutRelicData.Effects = GetRelicEffectData(InRelicID);
	return true;
}

TArray<FName> URelicSubsystem::GetAllRelicIDs() const
{
	TArray<FName> Result;

	if (!RelicDataTable)
	{
		return Result;
	}

	for (const FName& RowName : RelicDataTable->GetRowNames())
	{
		const FRelicDataRow* Row = RelicDataTable->FindRow<FRelicDataRow>(RowName, TEXT("URelicSubsystem::GetAllRelicIDs"));

		if (Row)
		{
			Result.Add(Row->RelicID);
		}
	}

	return Result;
}
