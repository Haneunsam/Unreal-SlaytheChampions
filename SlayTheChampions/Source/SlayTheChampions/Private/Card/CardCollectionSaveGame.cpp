#include "Card/CardCollectionSaveGame.h"
#include "Kismet/GameplayStatics.h"

const FString UCardCollectionSaveGame::SlotName = TEXT("CardCollectionSave");
const int32 UCardCollectionSaveGame::UserIndex = 0;

UCardCollectionSaveGame* UCardCollectionSaveGame::LoadOrCreate()
{
    // 기존 파일 있으면 로드
    if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
    {
        UCardCollectionSaveGame* Save = Cast<UCardCollectionSaveGame>(
            UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));

        if (Save)
        {
            UE_LOG(LogTemp, Warning,
                TEXT("[CardCollectionSaveGame] Loaded - %d cards unlocked."),
                Save->UnlockedCards.Num());
            return Save;
        }
    }

    // 파일 없으면 빈 SaveGame 새로 생성
    UCardCollectionSaveGame* NewSave = Cast<UCardCollectionSaveGame>(
        UGameplayStatics::CreateSaveGameObject(UCardCollectionSaveGame::StaticClass()));

    UE_LOG(LogTemp, Warning,
        TEXT("[CardCollectionSaveGame] No save found - Created new collection."));

    WriteSave(NewSave);
    return NewSave;
}

void UCardCollectionSaveGame::WriteSave(UCardCollectionSaveGame* SaveGame)
{
    if (!SaveGame) return;

    UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, UserIndex);

    UE_LOG(LogTemp, Warning,
        TEXT("[CardCollectionSaveGame] Saved - %d cards in collection."),
        SaveGame->UnlockedCards.Num());
}

void UCardCollectionSaveGame::UnlockCard(FName CardID)
{
    if (CardID.IsNone()) return;

    UCardCollectionSaveGame* Save = LoadOrCreate();
    if (!Save) return;

    // 이미 획득한 카드면 중복 추가 안 함
    if (Save->UnlockedCards.Contains(CardID))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[CardCollectionSaveGame] UnlockCard - '%s' already unlocked."),
            *CardID.ToString());
        return;
    }

    Save->UnlockedCards.Add(CardID);
    WriteSave(Save);

    UE_LOG(LogTemp, Warning,
        TEXT("[CardCollectionSaveGame] Unlocked '%s' (total %d)."),
        *CardID.ToString(), Save->UnlockedCards.Num());
}

bool UCardCollectionSaveGame::IsUnlocked(FName CardID)
{
    if (CardID.IsNone()) return false;

    UCardCollectionSaveGame* Save = LoadOrCreate();
    if (!Save) return false;

    return Save->UnlockedCards.Contains(CardID);
}

TArray<FName> UCardCollectionSaveGame::GetUnlockedCards()
{
    UCardCollectionSaveGame* Save = LoadOrCreate();
    if (!Save) return {};

    return Save->UnlockedCards;
}
