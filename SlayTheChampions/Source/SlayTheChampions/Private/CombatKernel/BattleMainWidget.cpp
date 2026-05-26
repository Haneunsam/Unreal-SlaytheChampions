#include "CombatKernel/BattleMainWidget.h"
#include "CombatKernel/CombatManager.h"
#include "Unit/Unit.h"
#include "Card/CardUserComponent.h"
#include "Card/CardSubsystem.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

// 위젯 초기화: CombatManager 탐색 및 바인딩, 플레이어 클릭 이벤트 바인딩, 마우스 활성화
void UBattleMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// CombatManager 자동 탐색
	CombatManager = Cast<ACombatManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ACombatManager::StaticClass()));

	if (CombatManager)
	{
		CombatManager->OnPhaseChanged.AddDynamic(this, &UBattleMainWidget::OnPhaseChanged);
		// BeginPlay에서 이미 StartTurn이 호출됐으므로 초기값 직접 설정
		if (Text_TurnCount)
			Text_TurnCount->SetText(FText::FromString(FString::Printf(TEXT("Turn %d"), CombatManager->TurnCount)));

		// SpawnedPlayers 클릭 이벤트 바인딩
		BindPlayerClickEvents();
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("[BattleMainWidget] CombatManager not found in level."));

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

// SpawnedPlayers 각각의 OnUnitClicked에 바인딩
void UBattleMainWidget::BindPlayerClickEvents()
{
	if (!CombatManager) return;

	for (AUnit* Unit : CombatManager->GetSpawnedPlayers())
	{
		if (Unit)
			Unit->OnUnitClicked.AddDynamic(this, &UBattleMainWidget::HandlePlayerClicked);
	}
}

// 플레이어 유닛 클릭 시 호출
// 이전 선택의 CardUserComponent 바인딩을 해제하고, 새 유닛의 CardUserComponent를 바인딩 후 현재 손패를 표시
void UBattleMainWidget::HandlePlayerClicked(AUnit* Unit)
{
	if (!Unit) return;
	UE_LOG(LogTemp, Log, TEXT("[BattleMainWidget] Player clicked: %s"), *Unit->GetName());

	// 이전 선택 유닛의 CardUserComponent 바인딩 해제
	if (SelectedUnit)
	{
		UCardUserComponent* PrevCard = SelectedUnit->FindComponentByClass<UCardUserComponent>();
		if (PrevCard)
			PrevCard->OnHandChanged.RemoveDynamic(this, &UBattleMainWidget::HandleHandChanged);
	}

	SelectedUnit = Unit;
	OnPlayerSelected(Unit);

	// 새 유닛의 CardUserComponent 바인딩 및 현재 손패 즉시 표시
	UCardUserComponent* CardComp = Unit->FindComponentByClass<UCardUserComponent>();
	if (CardComp)
	{
		CardComp->OnHandChanged.AddDynamic(this, &UBattleMainWidget::HandleHandChanged);
		HandleHandChanged(CardComp->GetHand());
	}
	else
	{
		// CardUserComponent가 없으면 빈 손패 전달
		OnHandUpdated(TArray<FCardDataRow>());
	}
}

// CardUserComponent::OnHandChanged 수신
// 카드 ID 목록을 CardSubsystem으로 조회해 FCardDataRow 배열로 변환 후 BP에 전달
void UBattleMainWidget::HandleHandChanged(const TArray<FName>& CardNames)
{
	UCardSubsystem* CS = GetGameInstance()
		? GetGameInstance()->GetSubsystem<UCardSubsystem>()
		: nullptr;

	TArray<FCardDataRow> Cards;

	if (CS)
	{
		for (const FName& Name : CardNames)
		{
			const FCardDataRow* Row = CS->GetCard(Name);
			if (Row) Cards.Add(*Row);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[BattleMainWidget] Hand updated: %d cards"), Cards.Num());
	for (const FCardDataRow& Card : Cards)
		UE_LOG(LogTemp, Log, TEXT("[BattleMainWidget]  - %s"), *Card.CardID.ToString());

	OnHandUpdated(Cards);
}
