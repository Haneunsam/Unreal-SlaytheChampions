#include "CombatKernel/BattleMainWidget.h"
#include "Card/CardWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UBattleMainWidget::AddCard(const FCardDataRow& InCardData)
{
	UCardWidget* LocalWidgetCard = CreateWidget<UCardWidget>(GetOwningPlayer(), NewCard);
	if (!LocalWidgetCard || !MainCanvas) return;

	LocalWidgetCard->SetCardData(InCardData, nullptr);

	UCanvasPanelSlot* CanvasSlot = MainCanvas->AddChildToCanvas(LocalWidgetCard);
	if (CanvasSlot) CanvasSlot->SetAutoSize(true);

	FWidgetCardsStruct NewEntry;
	NewEntry.CardWidget = LocalWidgetCard;
	NewEntry.Canvas     = MainCanvas;
	NewEntry.CardSlot   = CanvasSlot;
	WidgetCards.Add(NewEntry);
}

void UBattleMainWidget::OnCardExecuted_Implementation(const FCardDataRow& Card)
{
	// 카드 → CombatStatComponent 연결은 추후 구현
}

void UBattleMainWidget::HandleCardClicked(UCardWidget* Widget, const FCardDataRow& Card)
{
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

	SharedCost -= Card.Cost;
	OnCostChanged(SharedCost, MaxCost);
	OnCardExecuted(Card);
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
