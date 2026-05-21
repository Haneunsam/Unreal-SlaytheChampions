#include "CardSubsystem.h"
#include "Engine/DataTable.h"

// 서브시스템 Lifecycle -------------------------------------------------------

void UCardSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // 에셋 경로 기반 경로로 DataTable 자동 로드
    // 실제 경로는 프로젝트에 맞게 조정
    static const FSoftObjectPath CardTablePath(
        TEXT("01/DT_Cards"));

    if (UDataTable* Loaded = Cast<UDataTable>(CardTablePath.TryLoad()))
    {
        LoadCardDataTable(Loaded);
    }
    else
    {
        UE_LOG(LogTemp, Warning,
            TEXT("UCardSubsystem: DT_Cards not found at '%s'. "
                "Call LoadCardDataTable() manually."),
            *CardTablePath.ToString());
    }
}

// Public API -----------------------------------------------------------------

void UCardSubsystem::LoadCardDataTable(UDataTable* InTable)
{
    CardDataTable = InTable;

    if (!CardDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("UCardSubsystem: Null DataTable passed to LoadCardDataTable."));
        return;
    }

    UE_LOG(LogTemp, Log,
        TEXT("UCardSubsystem: Loaded DataTable '%s' (%d rows)."),
        *CardDataTable->GetName(),
        CardDataTable->GetRowNames().Num());
}

const FCardDataRow* UCardSubsystem::GetCard(FName RowName) const
{
    if (!CardDataTable) return nullptr;
    return CardDataTable->FindRow<FCardDataRow>(RowName, TEXT("UCardSubsystem::GetCard"));
}

TArray<FName> UCardSubsystem::GetCardNamesByClass(EJobClass JobClass) const
{
    TArray<FName> Result;
    if (!CardDataTable) return Result;

    for (const FName& RowName : CardDataTable->GetRowNames())
    {
        const FCardDataRow* Row =
            CardDataTable->FindRow<FCardDataRow>(RowName, TEXT("GetCardNamesByClass"));

        if (Row && (Row->RequiredClass == JobClass || Row->RequiredClass == EJobClass::Any))
        {
            Result.Add(RowName);
        }
    }
    return Result;
}

TArray<const FCardDataRow*> UCardSubsystem::GetCardsByClass(EJobClass JobClass) const
{
    TArray<const FCardDataRow*> Result;
    if (!CardDataTable) return Result;

    for (const FName& RowName : CardDataTable->GetRowNames())
    {
        const FCardDataRow* Row =
            CardDataTable->FindRow<FCardDataRow>(RowName, TEXT("GetCardsByClass"));

        if (Row && (Row->RequiredClass == JobClass || Row->RequiredClass == EJobClass::Any))
        {
            Result.Add(Row);
        }
    }
    return Result;
}

TArray<FName> UCardSubsystem::GetRewardPool(EJobClass JobClass, ECardRarity MinRarity) const
{
    TArray<FName> Result;
    if (!CardDataTable) return Result;

    const int32 MinRarityInt = RarityToInt(MinRarity);

    for (const FName& RowName : CardDataTable->GetRowNames())
    {
        const FCardDataRow* Row =
            CardDataTable->FindRow<FCardDataRow>(RowName, TEXT("GetRewardPool"));

        if (!Row) continue;

        const bool bClassOk =
            (Row->RequiredClass == JobClass || Row->RequiredClass == EJobClass::Any);
        const bool bRarityOk =
            (RarityToInt(Row->Rarity) >= MinRarityInt);

        if (bClassOk && bRarityOk)
        {
            Result.Add(RowName);
        }
    }
    return Result;
}

TArray<FName> UCardSubsystem::GetAllCardNames() const
{
    if (!CardDataTable) return {};
    return CardDataTable->GetRowNames();
}

// Private Helpers ------------------------------------------------------------

int32 UCardSubsystem::RarityToInt(ECardRarity Rarity)
{
    switch (Rarity)
    {
    case ECardRarity::Status:    return 0;
    case ECardRarity::Normal:    return 1;
    case ECardRarity::Rare:      return 2;
    case ECardRarity::Legendary: return 3;
    default:                     return 0;
    }
}
