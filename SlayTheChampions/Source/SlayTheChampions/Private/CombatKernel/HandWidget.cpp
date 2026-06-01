#include "CombatKernel/HandWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Card/CardWidget.h"

// 뽑을 덱 / 버린 덱 카운트 텍스트 갱신
void UHandWidget::UpdateDeckCounts(int32 DrawPile, int32 DiscardPile)
{
	if (Text_DrawCount)
		Text_DrawCount->SetText(FText::AsNumber(DrawPile));
	if (Text_DiscardCount)
		Text_DiscardCount->SetText(FText::AsNumber(DiscardPile));
}

// 손패 갱신: 기존 카드 제거 → BP에 카드 데이터 전달 → 레이아웃 적용
void UHandWidget::ShowHand(const TArray<FCardDataRow>& Cards)
{
	ClearHand();
	OnShowHand(Cards);
	ApplyCardLayout();
}

// 컨테이너의 자식 위젯 전부 제거
// ClearChildren 전에 각 카드에 MarkForRemoval을 호출해 Slate 디퍼드 이벤트를 차단
void UHandWidget::ClearHand()
{
	// CardCanvas 우선, 없으면 CardContainer(HorizontalBox) 사용
	UPanelWidget* Target = CardCanvas ? static_cast<UPanelWidget*>(CardCanvas)
	                                  : static_cast<UPanelWidget*>(CardContainer);
	if (!Target) return;

	for (int32 i = 0; i < Target->GetChildrenCount(); ++i)
	{
		if (UCardWidget* Card = Cast<UCardWidget>(Target->GetChildAt(i)))
			Card->MarkForRemoval();
	}

	Target->ClearChildren();
}

// OnShowHand 완료 후 CardCanvas의 카드 위치와 z-order를 설정
// - 5장 이하: CardSpacing 간격으로 나란히 배치
// - 6장 이상: OverlapFraction 비율로 겹쳐 배치, 오른쪽 카드가 위에 보임
// - 항상 캔버스 중앙 기준으로 정렬
void UHandWidget::ApplyCardLayout()
{
	if (!CardCanvas) return;

	const int32 Count = CardCanvas->GetChildrenCount();
	if (Count == 0) return;

	const bool bOverlap = Count > 5;
	const float Step = bOverlap
		? CardWidth * (1.f - OverlapFraction)
		: CardWidth + CardSpacing;

	// 전체 카드 열이 차지하는 너비
	const float TotalWidth = (Count - 1) * Step + CardWidth;

	// 캔버스 실제 너비 조회 — 아직 레이아웃 전이면 PanelWidth 폴백 사용
	const float CanvasW = CardCanvas->GetCachedGeometry().GetLocalSize().X;
	const float PanelW  = CanvasW > 0.f ? CanvasW : PanelWidth;

	// 중앙 정렬 시작 X
	const float StartX = (PanelW - TotalWidth) * 0.5f;

	for (int32 i = 0; i < Count; i++)
	{
		UWidget* Child = CardCanvas->GetChildAt(i);
		if (!Child) continue;

		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Child->Slot);
		if (!CanvasSlot) continue;

		// 오른쪽에서 왼쪽 순서: 인덱스 0이 오른쪽 끝, 마지막 인덱스가 왼쪽 끝
		const int32 Rev = Count - 1 - i;
		CanvasSlot->SetAutoSize(true);
		CanvasSlot->SetPosition(FVector2D(StartX + Rev * Step, 0.f));
		CanvasSlot->SetZOrder(Rev);
	}
}

// 호버 시 해당 카드를 z-order 최상위로 올림
// 이전에 호버된 카드가 있으면 강제 언호버 — 빠른 마우스 이동 시 stuck 방지
void UHandWidget::SetCardZOrderTop(UCardWidget* Card)
{
	if (!Card || !CardCanvas) return;

	// 다른 카드가 호버 상태로 남아있으면 강제 언호버
	if (HoveredCard && HoveredCard != Card && !HoveredCard->IsPendingState())
		HoveredCard->OnCardUnhovered();

	HoveredCard = Card;

	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Card->Slot);
	if (CanvasSlot)
		CanvasSlot->SetZOrder(CardCanvas->GetChildrenCount() + 100);
}

// 호버 해제 시 원래 인덱스 기반 z-order로 복원
void UHandWidget::RestoreCardZOrder(UCardWidget* Card)
{
	if (!Card || !CardCanvas) return;

	if (HoveredCard == Card)
		HoveredCard = nullptr;

	const int32 Total = CardCanvas->GetChildrenCount();
	for (int32 i = 0; i < Total; i++)
	{
		if (CardCanvas->GetChildAt(i) == Card)
		{
			UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Card->Slot);
			if (CanvasSlot) CanvasSlot->SetZOrder(Total - 1 - i);
			return;
		}
	}
}

// 타겟 지정 중 카드 인터랙션 차단/허용
// bIsTargeting=true: 모든 카드를 HitTestInvisible로 전환 (클릭·호버 차단)
// bIsTargeting=false: 모든 카드를 Visible로 복원
void UHandWidget::SetTargetingMode(bool bInTargeting)
{
	bIsTargeting = bInTargeting;

	UPanelWidget* Target = CardCanvas
		? static_cast<UPanelWidget*>(CardCanvas)
		: static_cast<UPanelWidget*>(CardContainer);
	if (!Target) return;

	const ESlateVisibility CardVis = bInTargeting
		? ESlateVisibility::HitTestInvisible
		: ESlateVisibility::Visible;

	for (int32 i = 0; i < Target->GetChildrenCount(); i++)
	{
		if (UWidget* Card = Target->GetChildAt(i))
			Card->SetVisibility(CardVis);
	}
}

// 마우스가 핸드 패널에 진입하면 대기 카드 호버 상태 해제
// 타겟 지정 중에는 상호작용 차단 — 선택된 카드 호버 상태 유지
void UHandWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (!bIsTargeting)
		ClearCardPending();
}

// 위젯 포인터를 직접 받아 대기 상태 진입
void UHandWidget::SetCardPendingDirect(UCardWidget* Card)
{
	ClearCardPending();
	if (!Card) return;
	PendingCard = Card;
	Card->SetPendingState(true);
}

// 현재 대기 카드의 상태 해제 및 참조 초기화
void UHandWidget::ClearCardPending()
{
	if (PendingCard)
	{
		PendingCard->SetPendingState(false);
		PendingCard = nullptr;
	}
}
