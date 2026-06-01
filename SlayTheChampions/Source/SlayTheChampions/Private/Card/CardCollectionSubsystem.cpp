#include "Card/CardCollectionSubsystem.h"
#include "Card/CardCollectionSaveGame.h"
#include "Card/CardSubsystem.h"

// ── Subsystem 라이프사이클 ──────────────────────────────────────────────────────

void UCardCollectionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // 게임 시작 시 SaveGame 에서 획득 목록 로드해 캐시에 저장
    CachedUnlockedCards = UCardCollectionSaveGame::GetUnlockedCards();

    UE_LOG(LogTemp, Warning,
        TEXT("[CardCollectionSubsystem] Initialized - %d cards unlocked."),
        CachedUnlockedCards.Num());
}

// ── 획득 등록 ─────────────────────────────────────────────────────────────────

void UCardCollectionSubsystem::UnlockCard(FName CardID)
{
    if (CardID.IsNone()) return;

    // 이미 캐시에 있으면 중복 처리 안 함
    if (CachedUnlockedCards.Contains(CardID))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[CardCollectionSubsystem] UnlockCard - '%s' already unlocked."),
            *CardID.ToString());
        return;
    }

    // 캐시에 추가 후 SaveGame 에 저장 (디스크 반영)
    CachedUnlockedCards.Add(CardID);
    UCardCollectionSaveGame::UnlockCard(CardID);

    UE_LOG(LogTemp, Warning,
        TEXT("[CardCollectionSubsystem] Unlocked '%s' (total %d)."),
        *CardID.ToString(), CachedUnlockedCards.Num());
}

// ── 획득 조회 ─────────────────────────────────────────────────────────────────

bool UCardCollectionSubsystem::IsUnlocked(FName CardID) const
{
    // 캐시에서 바로 조회 (디스크 접근 없음)
    return CachedUnlockedCards.Contains(CardID);
}

ECardLockState UCardCollectionSubsystem::GetLockState(FName CardID) const
{
    // 획득 완료면 바로 반환
    if (IsUnlocked(CardID))
        return ECardLockState::Unlocked;

    // 미획득 - 업적 전용 카드 여부 확인
    UCardSubsystem* CardSS = GetGameInstance()->GetSubsystem<UCardSubsystem>();
    if (CardSS)
    {
        const FCardDataRow* Row = CardSS->GetCard(CardID);
        if (Row && Row->bAchievementOnly)
            return ECardLockState::LockedAchievement;
    }

    return ECardLockState::LockedNormal;
}

TArray<FName> UCardCollectionSubsystem::GetUnlockedCards() const
{
    return CachedUnlockedCards;
}

// ── 도감 필터 조회 ────────────────────────────────────────────────────────────

TArray<FCardCollectionEntry> UCardCollectionSubsystem::GetFilteredCards(const FCardCollectionFilter& Filter) const
{
    UCardSubsystem* CardSS = GetGameInstance()->GetSubsystem<UCardSubsystem>();
    if (!CardSS)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[CardCollectionSubsystem] GetFilteredCards - CardSubsystem not found."));
        return {};
    }

    TArray<FName> AllCards = CardSS->GetAllCardNames();
    TArray<FCardCollectionEntry> Result;

    for (const FName& CardID : AllCards)
    {
        const FCardDataRow* Row = CardSS->GetCard(CardID);
        if (!Row) continue;

        // 직업 필터: 비어있으면 전체 허용. Any 카드는 항상 통과.
        if (Filter.JobClasses.Num() > 0)
        {
            bool bJobMatch = (Row->RequiredClass == EJobClass::Any)
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

        Result.Add(MakeEntry(CardID, IsUnlocked(CardID), Row->bAchievementOnly));
    }

    UE_LOG(LogTemp, Warning,
        TEXT("[CardCollectionSubsystem] GetFilteredCards - %d / %d cards matched."),
        Result.Num(), AllCards.Num());

    return Result;
}

TArray<FCardCollectionEntry> UCardCollectionSubsystem::GetAllCards() const
{
    // 필터 없이 전체 카드 목록 반환
    UCardSubsystem* CardSS = GetGameInstance()->GetSubsystem<UCardSubsystem>();
    if (!CardSS) return {};

    TArray<FName> AllCards = CardSS->GetAllCardNames();
    TArray<FCardCollectionEntry> Result;
    Result.Reserve(AllCards.Num());

    for (const FName& CardID : AllCards)
    {
        const FCardDataRow* Row = CardSS->GetCard(CardID);
        Result.Add(MakeEntry(CardID, IsUnlocked(CardID), Row ? Row->bAchievementOnly : false));
    }

    return Result;
}

// ── Private ──────────────────────────────────────────────────────────────────

FCardCollectionEntry UCardCollectionSubsystem::MakeEntry(FName CardID, bool bUnlocked, bool bAchievement) const
{
    FCardCollectionEntry Entry;
    Entry.CardID = CardID;

    if (bUnlocked)
    {
        // 획득 완료
        Entry.LockState = ECardLockState::Unlocked;
    }
    else if (bAchievement)
    {
        // 미획득 업적 전용 카드 - 완전 검정
        Entry.LockState = ECardLockState::LockedAchievement;
    }
    else
    {
        // 미획득 일반 카드 - 실루엣
        Entry.LockState = ECardLockState::LockedNormal;
    }

    return Entry;
}
