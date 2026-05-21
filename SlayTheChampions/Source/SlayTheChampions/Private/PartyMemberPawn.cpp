#include "PartyMemberPawn.h"
#include "HandComponent.h"
#include "CardSubsystem.h"
#include "CardSaveGame.h"

APartyMemberPawn::APartyMemberPawn()
{
    PrimaryActorTick.bCanEverTick = false;

    // HandComponent 생성 및 등록
    HandComponent = CreateDefaultSubobject<UHandComponent>(TEXT("HandComponent"));
}

void APartyMemberPawn::BeginPlay()
{
    Super::BeginPlay();

    // 게임 시작 시 자동으로 덱 초기화
    InitializeDeck();
}

void APartyMemberPawn::InitializeDeck()
{
    if (!HandComponent) return;

    TArray<FName> DeckNames;

    // ── 1순위: SaveGame 에서 덱 불러오기 ─────────────────────────────────
    // 전투 종료 후 저장된 덱이 있으면 그것을 사용
    TArray<FName> SavedDeck = UCardSaveGame::GetDeckCards(PawnIndex);
    if (SavedDeck.Num() > 0)
    {
        DeckNames = SavedDeck;
        UE_LOG(LogTemp, Log, TEXT("[%s] SaveGame 에서 덱 로드 - %d장"), *GetName(), DeckNames.Num());
    }

    // ── 2순위: OverrideDeckNames 직접 지정 (테스트용) ────────────────────
    if (DeckNames.IsEmpty() && OverrideDeckNames.Num() > 0)
    {
        DeckNames = OverrideDeckNames;
        UE_LOG(LogTemp, Log, TEXT("[%s] OverrideDeck 사용 - %d장"), *GetName(), DeckNames.Num());
    }

    // ── 3순위: CardSubsystem 에서 직업 기반 자동 조회 ────────────────────
    if (DeckNames.IsEmpty())
    {
        if (UGameInstance* GI = GetGameInstance())
        {
            if (UCardSubsystem* CS = GI->GetSubsystem<UCardSubsystem>())
            {
                DeckNames = CS->GetCardNamesByClass(JobClass);
                UE_LOG(LogTemp, Log, TEXT("[%s] CardSubsystem 에서 덱 로드 - %d장"), *GetName(), DeckNames.Num());
            }
        }
    }

    // 덱이 비어있으면 초기화 중단
    if (DeckNames.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] InitializeDeck: 카드를 찾을 수 없음"), *GetName());
        return;
    }

    // HandComponent 에 덱 전달 (내부에서 셔플 후 DrawPile 설정)
    HandComponent->InitializeDeck(DeckNames);
    UE_LOG(LogTemp, Log, TEXT("[%s] 덱 초기화 완료 - 총 %d장"), *GetName(), DeckNames.Num());
}

void APartyMemberPawn::SaveDeckToSaveGame()
{
    if (!HandComponent) return;

    // DrawPile + Hand + DiscardPile 을 SaveGame 에 저장 (A방식)
    UCardSaveGame::SaveDeckAfterBattle(
        PawnIndex,
        HandComponent->GetDrawPile(),
        HandComponent->GetHand(),
        HandComponent->GetDiscardPile()
    );
}

void APartyMemberPawn::DrawStartOfTurn()
{
    if (!HandComponent) return;
    HandComponent->DrawStartOfTurn();
}

bool APartyMemberPawn::PlayCard(FName CardName)
{
    if (!HandComponent) return false;
    return HandComponent->PlayCard(CardName);
}

void APartyMemberPawn::DiscardHand()
{
    if (!HandComponent) return;
    HandComponent->DiscardHand();
}

TArray<FName> APartyMemberPawn::GetHand() const
{
    return HandComponent ? HandComponent->GetHand() : TArray<FName>();
}

int32 APartyMemberPawn::GetDrawPileCount() const
{
    return HandComponent ? HandComponent->GetDrawPileCount() : 0;
}

int32 APartyMemberPawn::GetDiscardPileCount() const
{
    return HandComponent ? HandComponent->GetDiscardPileCount() : 0;
}