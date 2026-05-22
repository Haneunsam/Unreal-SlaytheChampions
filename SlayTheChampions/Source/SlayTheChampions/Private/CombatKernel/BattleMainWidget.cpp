#include "CombatKernel/BattleMainWidget.h"
#include "CombatKernel/CombatManager.h"
#include "Card/CardWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

// 위젯 초기화: HandComponent 생성, CombatManager 탐색 및 바인딩, 마우스 활성화
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

// DrawPhase 진입 시 턴 카운트 텍스트를 갱신
void UBattleMainWidget::OnPhaseChanged(ETurnPhase NewPhase)
{
	if (NewPhase != ETurnPhase::DrawPhase || !CombatManager) return;

	if (Text_TurnCount)
		Text_TurnCount->SetText(FText::FromString(FString::Printf(TEXT("Turn %d"), CombatManager->TurnCount)));
}

// 카드 위젯을 생성하고 HandCanvas에 추가. WidgetCards 목록에도 등록
void UBattleMainWidget::AddCard(const FCardDataRow& InCardData)
{
	UCardWidget* LocalWidgetCard = CreateWidget<UCardWidget>(GetOwningPlayer(), NewCard);
	if (!LocalWidgetCard || !HandCanvas) return;

	UE_LOG(LogTemp, Warning, TEXT("[AddCard] Cost=%d Damage=%d Heal=%d Draw=%d"), InCardData.Cost, InCardData.Damage, InCardData.HealAmount, InCardData.DrawCount);

	LocalWidgetCard->SetCardData(InCardData, nullptr);

	UCanvasPanelSlot* CanvasSlot = HandCanvas->AddChildToCanvas(LocalWidgetCard);
	if (CanvasSlot) CanvasSlot->SetAutoSize(true);

	// 위젯 추적 목록에 등록
	FWidgetCardsStruct NewEntry;
	NewEntry.CardWidget = LocalWidgetCard;
	NewEntry.Canvas     = HandCanvas;
	NewEntry.CardSlot   = CanvasSlot;
	WidgetCards.Add(NewEntry);
}

// 카드 효과를 CombatManager에 위임하고, 드로우 효과가 있으면 HandComponent를 통해 처리
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

// 카드 클릭 시 호출. 페이즈·코스트 검증 후 카드를 큐에 추가하고 UI에서 제거
void UBattleMainWidget::HandleCardClicked(UCardWidget* Widget, const FCardDataRow& Card)
{
	// PlayerActionPhase일 때만 카드 사용 가능
	if (!CombatManager || CombatManager->CurrentPhase != ETurnPhase::PlayerActionPhase) return;
	// 코스트 부족 시 사용 불가
	if (SharedCost < Card.Cost) return;

	// 클릭한 카드 위젯을 WidgetCards에서 찾아 UI에서 제거
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
		Hand->PlayCard(Card.CardID);

	// 코스트 차감 후 UI 갱신
	SharedCost -= Card.Cost;
	OnCostChanged(SharedCost, MaxCost);

	// 즉시 실행 대신 큐에 추가
	CombatManager->QueuePlayerAction(Card, 0);

	OrganizeCards(20.0f);
}

// 손패 카드들을 화면 하단에 일정 간격으로 정렬
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
		// OffsetX 간격으로 카드를 가로로 배치
		CardSlot->SetPosition(FVector2D(OutBottomPadding + OffsetX * i, OutCenterOfScreen));
		CardSlot->SetZOrder(i + 1);
	}
}

// 뷰포트 크기를 기반으로 카드 배치에 필요한 X 시작점(OutBottomPadding)과 Y 위치(OutCenterOfScreen)를 계산
void UBattleMainWidget::FN_GetScreenInfo(float InBottomMargin, float InCardHeight, float& OutBottomPadding, float& OutCenterOfScreen)
{
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	// X의 1/3 지점을 카드 배치 시작 위치로 사용
	OutCenterOfScreen = ViewportSize.X / 3.0f;
	// 화면 하단에서 BottomMargin + 카드 높이 1.5배 위에 배치
	OutBottomPadding  = (ViewportSize.Y - InBottomMargin) - (InCardHeight * 1.5f);
}
