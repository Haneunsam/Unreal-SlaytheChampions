#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StarterDeckRow.generated.h"

/**
 * FStarterDeckRow
 *
 * 직업별 시작 덱 구성 DataTable Row Struct.
 * DT_StarterDeck_Warrior, DT_StarterDeck_Mage 등에서 사용.
 * CardID 는 DT_Cards 의 RowName 과 일치해야 한다.
 */
USTRUCT(BlueprintType)
struct FStarterDeckRow : public FTableRowBase
{
    GENERATED_BODY()

    // DT_Cards 의 RowName (Warrior_Attack, Warrior_Defend 등)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Deck")
    FName CardID;
};