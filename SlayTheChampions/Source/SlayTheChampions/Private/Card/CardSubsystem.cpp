#include "Card/CardSubsystem.h"
#include "Engine/DataTable.h"

// ── Lifecycle ────────────────────────────────────────────────────────────────

void UCardSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // 전체 카드 풀 DataTable 자동 로드 (경로 확정)
    static const FSoftObjectPath CardTablePath(
        TEXT("/Game/01_Card/01_Test_DT/DT_Cards.DT_Cards"));

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

// ── Public API ───────────────────────────────────────────────────────────────

void UCardSubsystem::LoadCardDataTable(UDataTable* InTable)
{
    CardDataTable = InTable;

    if (!CardDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("UCardSubsystem: Null DataTable passed to LoadCardDataTable."));
        return;
    }

    // CardID 필드값 → Row Name 역조회 캐시 구축
    CardIDToRowName.Empty();
    for (const FName& RowName : CardDataTable->GetRowNames())
    {
        const FCardDataRow* Row = CardDataTable->FindRow<FCardDataRow>(RowName, TEXT("BuildCardIDCache"));
        if (Row) CardIDToRowName.Add(Row->CardID, RowName);
    }

    UE_LOG(LogTemp, Log,
        TEXT("UCardSubsystem: Loaded DataTable '%s' (%d rows)."),
        *CardDataTable->GetName(),
        CardDataTable->GetRowNames().Num());
}

// CardID 필드값으로 Row Name을 역조회. 없으면 NAME_None 반환
FName UCardSubsystem::GetRowNameByCardID(FName CardID) const
{
    const FName* Found = CardIDToRowName.Find(CardID);
    return Found ? *Found : NAME_None;
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

// ── Private Helpers ──────────────────────────────────────────────────────────

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
