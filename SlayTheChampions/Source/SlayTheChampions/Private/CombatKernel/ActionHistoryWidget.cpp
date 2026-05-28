#include "CombatKernel/ActionHistoryWidget.h"
#include "Kismet/GameplayStatics.h"

// NativeConstruct: 레벨에서 CombatManager를 탐색해 델리게이트를 바인딩
void UActionHistoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 레벨에 배치된 CombatManager를 자동 탐색
	ACombatManager* Found = Cast<ACombatManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ACombatManager::StaticClass()));

	if (Found)
		SetCombatManager(Found);
	else
		UE_LOG(LogTemp, Warning, TEXT("[ActionHistoryWidget] CombatManager not found in level"));
}

// NativeDestruct: 위젯 소멸 시 델리게이트 구독 해제
void UActionHistoryWidget::NativeDestruct()
{
	if (CombatManager)
	{
		CombatManager->OnActionExecuted.RemoveDynamic(this, &UActionHistoryWidget::HandleActionExecuted);
		CombatManager->OnPhaseChanged.RemoveDynamic(this,  &UActionHistoryWidget::HandlePhaseChanged);
		CombatManager = nullptr;
	}

	Super::NativeDestruct();
}

// CombatManager를 수동으로 설정하고 델리게이트를 바인딩
void UActionHistoryWidget::SetCombatManager(ACombatManager* Manager)
{
	// 기존 바인딩 해제
	if (CombatManager)
	{
		CombatManager->OnActionExecuted.RemoveDynamic(this, &UActionHistoryWidget::HandleActionExecuted);
		CombatManager->OnPhaseChanged.RemoveDynamic(this,  &UActionHistoryWidget::HandlePhaseChanged);
	}

	CombatManager = Manager;

	if (CombatManager)
	{
		CombatManager->OnActionExecuted.AddDynamic(this, &UActionHistoryWidget::HandleActionExecuted);
		CombatManager->OnPhaseChanged.AddDynamic(this,  &UActionHistoryWidget::HandlePhaseChanged);
	}
}

// OnActionExecuted 수신: 새 항목 추가 이벤트를 BP에 전달
void UActionHistoryWidget::HandleActionExecuted(FCardDataRow Card, int32 CasterIndex)
{
	OnAddEntry(Card, CasterIndex);
}

// OnPhaseChanged 수신: DrawPhase 진입 시 히스토리 초기화
void UActionHistoryWidget::HandlePhaseChanged(ETurnPhase NewPhase)
{
	if (NewPhase == ETurnPhase::DrawPhase)
		OnClearHistory();
}
