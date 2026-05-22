#include "Card/PartyMemberPawn.h"
#include "Card/HandComponent.h"
#include "Card/DeckComponent.h"
#include "Card/CardSubsystem.h"
#include "Card/CardSaveGame.h"
#include "GameManagers/CardManager.h"

APartyMemberPawn::APartyMemberPawn()
{
    PrimaryActorTick.bCanEverTick = false;

    // DeckComponent 먼저 생성 (HandComponent 초기화 시 참조 필요)
    DeckComponent = CreateDefaultSubobject<UDeckComponent>(TEXT("DeckComponent"));
    HandComponent = CreateDefaultSubobject<UHandComponent>(TEXT("HandComponent"));
}

void APartyMemberPawn::BeginPlay()
{
    Super::BeginPlay();

    // DeckComponent 를 CardManager 에 등록
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UCardManager* CM = GI->GetSubsystem<UCardManager>())
        {
            CM->RegisterDeckComponent(PawnIndex, DeckComponent);
        }
    }

    // 게임 시작 시 자동으로 덱 초기화
    InitializeDeck();
}

void APartyMemberPawn::InitializeDeck()
{
    if (!HandComponent || !DeckComponent) return;

    TArray<FName> DeckNames;

    // ── 1순위: SaveGame 에서 덱 불러오기 ─────────────────────────────────────
    TArray<FName> SavedDeck = UCardSaveGame::GetDeckCards(PawnIndex);
    if (SavedDeck.Num() > 0)
    {
        DeckNames = SavedDeck;
        UE_LOG(LogTemp, Log, TEXT("[%s] Loaded deck from SaveGame - %d cards."), *GetName(), DeckNames.Num());
    }

    // ── 2순위: OverrideDeckNames 직접 지정 (테스트용) ─────────────────────────
    if (DeckNames.IsEmpty() && OverrideDeckNames.Num() > 0)
    {
        DeckNames = OverrideDeckNames;
        UE_LOG(LogTemp, Log, TEXT("[%s] Using OverrideDeck - %d cards."), *GetName(), DeckNames.Num());
    }

    // ── 3순위: CardSubsystem 에서 직업 기반 자동 조회 ─────────────────────────
    if (DeckNames.IsEmpty())
    {
        if (UGameInstance* GI = GetGameInstance())
        {
            if (UCardSubsystem* CS = GI->GetSubsystem<UCardSubsystem>())
            {
                DeckNames = CS->GetCardNamesByClass(JobClass);
                UE_LOG(LogTemp, Log, TEXT("[%s] Loaded deck from CardSubsystem - %d cards."), *GetName(), DeckNames.Num());
            }
        }
    }

    if (DeckNames.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] InitializeDeck: No cards found. Deck not initialized."), *GetName());
        return;
    }

    // HandComponent 에 DeckComponent 와 카드 목록 전달 (내부에서 셔플/초기화)
    HandComponent->InitializeDeck(DeckComponent, DeckNames);
    UE_LOG(LogTemp, Log, TEXT("[%s] Deck initialized. Total %d cards."), *GetName(), DeckNames.Num());
}

void APartyMemberPawn::SaveDeckToSaveGame()
{
    if (!HandComponent || !DeckComponent) return;

    // DrawPile + Hand + DiscardPile 을 합산해 저장 (A방식)
    UCardSaveGame::SaveDeckAfterBattle(
        PawnIndex,
        DeckComponent->GetDrawPile(),
        HandComponent->GetHand(),
        DeckComponent->GetDiscardPile()
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
    return DeckComponent ? DeckComponent->GetDrawPileCount() : 0;
}

int32 APartyMemberPawn::GetDiscardPileCount() const
{
    return DeckComponent ? DeckComponent->GetDiscardPileCount() : 0;
}
