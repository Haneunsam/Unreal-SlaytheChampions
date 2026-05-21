#include "CombatKernel/BattleMainWidget.h"
#include "CombatKernel/CombatManager.h"
#include "CardWidget.h"
#include "HandComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UBattleMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 손패 컴포넌트 생성
	Hand = NewObject<UHandComponent>(this, UHandComponent::StaticClass(), TEXT("HandComponent"));

	// CombatManager 자동 탐색
	CombatManager = Cast<ACombatManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ACombatManager::StaticClass()));

	if (CombatManager)
	{
		CombatManager->OnPhaseChanged.AddDynamic(this, &UBattleMainWidget::OnPhaseChanged);
		// BeginPlay에서 이미 StartTurn이 호출됐으므로 초기값 직접 설정
		if (Text_TurnCount)
			Text_TurnCount->SetText(FText::FromString(FString::Printf(TEXT("Turn %d"), CombatManager->TurnCount)));
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("[BattleMainWidget] CombatManager를 레벨에서 찾지 못했습니다."));

	// 마우스 커서 표시
	UMouseManager* MouseManager = GetGameInstance()->GetSubsystem<UMouseManager>();
	if (MouseManager)
		MouseManager->SetMouseVisibility(GetOwningPlayer(), true);
}

void UBattleMainWidget::OnPhaseChanged(ETurnPhase NewPhase)
{
	if (NewPhase != ETurnPhase::DrawPhase || !CombatManager) return;

	if (Text_TurnCount)
		Text_TurnCount->SetText(FText::FromString(FString::Printf(TEXT("Turn %d"), CombatManager->TurnCount)));
}

void UBattleMainWidget::AddCard(const FCardDataRow& InCardData)
{
	UCardWidget* LocalWidgetCard = CreateWidget<UCardWidget>(GetOwningPlayer(), NewCard);
	if (!LocalWidgetCard || !HandCanvas) return;

	UE_LOG(LogTemp, Warning, TEXT("[AddCard] Cost=%d Damage=%d Heal=%d Draw=%d"), InCardData.Cost, InCardData.Damage, InCardData.HealAmount, InCardData.DrawCount);

	LocalWidgetCard->SetCardData(InCardData, nullptr);

	UCanvasPanelSlot* CanvasSlot = HandCanvas->AddChildToCanvas(LocalWidgetCard);
	if (CanvasSlot) CanvasSlot->SetAutoSize(true);

	FWidgetCardsStruct NewEntry;
	NewEntry.CardWidget = LocalWidgetCard;
	NewEntry.Canvas     = HandCanvas;
	NewEntry.CardSlot   = CanvasSlot;
	WidgetCards.Add(NewEntry);
}

void UBattleMainWidget::OnCardExecuted_Implementation(const FCardDataRow& Card)
{
	UE_LOG(LogTemp, Warning, TEXT("[OnCardExecuted] CombatManager=%s Damage=%d"), CombatManager ? TEXT("OK") : TEXT("NULL"), Card.Damage);
	// 카드 효과 실행 (CombatManager에 위임)
	if (CombatManager)
		CombatManager->ExecuteCard(Card, 0);

	// 카드 드로우 효과 처리
	if (Card.DrawCount > 0 && Hand)
	{
		TArray<FName> Drawn = Hand->DrawCards(Card.DrawCount);
		// [임시] 드로우한 카드 UI 추가 — HandComponent ↔ UI 연결 구현 시 처리
	}
}

void UBattleMainWidget::HandleCardClicked(UCardWidget* Widget, const FCardDataRow& Card)
{
	// PlayerActionPhase일 때만 카드 사용 가능
	if (!CombatManager || CombatManager->CurrentPhase != ETurnPhase::PlayerActionPhase) return;
	if (SharedCost < Card.Cost) return;

	const int32 RemoveIndex = WidgetCards.IndexOfByPredicate([&](const FWidgetCardsStruct& Entry)
	{
		return Entry.CardWidget == Widget;
	});

	if (RemoveIndex != INDEX_NONE)
	{
		if (WidgetCards[RemoveIndex].CardWidget)
			WidgetCards[RemoveIndex].CardWidget->RemoveFromParent();
		WidgetCards.RemoveAt(RemoveIndex);
	}

	if (Hand)
		Hand->PlayCard(Card.Name);

	SharedCost -= Card.Cost;
	OnCostChanged(SharedCost, MaxCost);

	// 즉시 실행 대신 큐에 추가
	CombatManager->QueuePlayerAction(Card, 0);

	OrganizeCards(20.0f);
}

void UBattleMainWidget::OrganizeCards(float OffsetX)
{
	float OutBottomPadding = 0.f;
	float OutCenterOfScreen = 0.f;
	FN_GetScreenInfo(BottomMargin, CardHeight, OutBottomPadding, OutCenterOfScreen);

	for (int32 i = 0; i < WidgetCards.Num(); i++)
	{
		UCanvasPanelSlot* CardSlot = WidgetCards[i].CardSlot;
		if (!CardSlot) continue;

		CardSlot->SetAutoSize(false);
		CardSlot->SetSize(FVector2D(CardWidth, CardHeight));
		CardSlot->SetPosition(FVector2D(OutBottomPadding + OffsetX * i, OutCenterOfScreen));
		CardSlot->SetZOrder(i + 1);
	}
}

void UBattleMainWidget::FN_GetScreenInfo(float InBottomMargin, float InCardHeight, float& OutBottomPadding, float& OutCenterOfScreen)
{
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	OutCenterOfScreen = ViewportSize.X / 3.0f;
	OutBottomPadding  = (ViewportSize.Y - InBottomMargin) - (InCardHeight * 1.5f);
}
