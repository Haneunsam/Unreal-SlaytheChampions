#include "CombatKernel/HandWidget.h"
#include "Components/HorizontalBox.h"
#include "Card/CardWidget.h"

// 손패 갱신: 기존 카드 제거 후 BP에 카드 데이터 전달
void UHandWidget::ShowHand(const TArray<FCardDataRow>& Cards)
{
	ClearHand();
	OnShowHand(Cards);
}

// CardContainer의 자식 위젯 전부 제거
// ClearChildren 전에 각 카드에 MarkForRemoval을 호출해 Slate 디퍼드 이벤트를 차단
// — UE5에서 ClearChildren 내부 실행 순서에 따라 Slot=nullptr 설정 전에
//   Slate MouseLeave가 발화될 수 있으므로 플래그를 먼저 설정
void UHandWidget::ClearHand()
{
	if (!CardContainer) return;

	// 제거 전 모든 카드 위젯에 플래그 설정 — 이후 발화되는 Slate 입력 이벤트 무시
	for (int32 i = 0; i < CardContainer->GetChildrenCount(); ++i)
	{
		if (UCardWidget* Card = Cast<UCardWidget>(CardContainer->GetChildAt(i)))
			Card->MarkForRemoval();
	}

	CardContainer->ClearChildren();
}

// 위젯 포인터를 직접 받아 대기 상태 진입 — 동일 CardID가 여러 장일 때 인덱스 0번이 잘못 선택되는 문제 방지
void UHandWidget::SetCardPendingDirect(UCardWidget* Card)
{
	ClearCardPending();
	if (!Card) return;
	PendingCard = Card;
	Card->SetPendingState(true);
}

// CardID와 일치하는 카드 위젯에 대기 상태 진입
// 이전 대기 카드가 있으면 먼저 해제 후 새 카드에 설정
void UHandWidget::SetCardPending(FName CardID)
{
	ClearCardPending();
	if (!CardContainer) return;

	for (int32 i = 0; i < CardContainer->GetChildrenCount(); ++i)
	{
		UCardWidget* Card = Cast<UCardWidget>(CardContainer->GetChildAt(i));
		if (Card && Card->GetCardID() == CardID)
		{
			PendingCard = Card;
			Card->SetPendingState(true);
			break;
		}
	}
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
