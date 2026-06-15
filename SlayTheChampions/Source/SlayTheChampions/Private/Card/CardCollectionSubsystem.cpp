#include "Card/CardCollectionSubsystem.h"
#include "Card/CardSubsystem.h"

TArray<FCardDataRow> UCardCollectionSubsystem::GetAllCards() const
{
    UCardSubsystem* CardSS = GetGameInstance()->GetSubsystem<UCardSubsystem>();
    if (!CardSS) return {};

    TArray<FName> AllNames = CardSS->GetAllCardNames();
    TArray<FCardDataRow> Result;
    Result.Reserve(AllNames.Num());

    for (const FName& Name : AllNames)
    {
        const FCardDataRow* Row = CardSS->GetCard(Name);
        if (Row) Result.Add(*Row);
    }

    return Result;
}

TArray<FCardDataRow> UCardCollectionSubsystem::GetFilteredCards(const FCardCollectionFilter& Filter) const
{
    UCardSubsystem* CardSS = GetGameInstance()->GetSubsystem<UCardSubsystem>();
    if (!CardSS) return {};

    TArray<FName> AllNames = CardSS->GetAllCardNames();
    TArray<FCardDataRow> Result;

    for (const FName& Name : AllNames)
    {
        const FCardDataRow* Row = CardSS->GetCard(Name);
        if (!Row) continue;

        // 직업 필터: 비어있으면 전체 허용. Any 카드는 항상 통과.
        if (Filter.JobClasses.Num() > 0)
        {
            const bool bJobMatch = (Row->RequiredClass == EJobClass::Any)
                || Filter.JobClasses.Contains(Row->RequiredClass);
            if (!bJobMatch) continue;
        }

        // 희귀도 필터: 비어있으면 전체 허용
        if (Filter.Rarities.Num() > 0)
        {
            if (!Filter.Rarities.Contains(Row->Rarity)) continue;
        }

        // 카드 타입 필터: 비어있으면 전체 허용
        if (Filter.CardTypes.Num() > 0)
        {
            if (!Filter.CardTypes.Contains(Row->CardType)) continue;
        }

        Result.Add(*Row);
    }

    UE_LOG(LogTemp, Log, TEXT("[CardCollectionSubsystem] GetFilteredCards - %d / %d cards matched."),
        Result.Num(), AllNames.Num());

    return Result;
}
