#include "CombatKernel/BattleMainWidget.h"
#include "CombatKernel/CombatManager.h"
#include "CombatKernel/HandWidget.h"
#include "Unit/Unit.h"
#include "Card/CardUserComponent.h"
#include "Card/CardSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Widget.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/ScopeExit.h"

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
		UpdateCostDisplay();

		// SpawnedPlayers 클릭 이벤트 바인딩
		BindPlayerClickEvents();
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("[BattleMainWidget] CombatManager not found in level."));

	// HandPanel 카드 선택 이벤트 바인딩
	if (HandPanel)
		HandPanel->OnCardSelected.AddDynamic(this, &UBattleMainWidget::HandleCardClicked);

	// 버튼 바인딩 및 초기 가시성 설정
	if (Btn_EndTurn)
		Btn_EndTurn->OnClicked.AddDynamic(this, &UBattleMainWidget::HandleEndTurnClicked);
	else
		UE_LOG(LogTemp, Error, TEXT("[BattleMainWidget] Btn_EndTurn is NULL — WBP 버튼 이름이 'Btn_EndTurn'인지 확인"));

	if (Btn_NextPlayer)
	{
		Btn_NextPlayer->OnClicked.AddDynamic(this, &UBattleMainWidget::HandleNextPlayerClicked);
		// 플레이어 수가 1명이면 숨김, 2명 이상이면 핸드 선택 후 표시 (초기엔 숨김)
		Btn_NextPlayer->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Btn_Back)
	{
		Btn_Back->OnClicked.AddDynamic(this, &UBattleMainWidget::HandleBackClicked);
		// 플레이어 선택 전 메인 화면에서는 숨김
		Btn_Back->SetVisibility(ESlateVisibility::Collapsed);
	}

	// SpawnedEnemies 클릭 이벤트 바인딩
	BindEnemyClickEvents();

	// 마우스 커서 표시
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UMouseManager* MouseManager = GI->GetSubsystem<UMouseManager>())
			MouseManager->SetMouseVisibility(GetOwningPlayer(), true);
	}

	// 평소엔 클릭 통과 — 카드 대기 상태 진입 시에만 Visible로 전환
	if (MainCanvas)
		MainCanvas->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

// 페이즈 전환 시 호출
// DrawPhase → 턴 카운트 텍스트 갱신, PlayerActionPhase → 코스트 리셋
void UBattleMainWidget::OnPhaseChanged(ETurnPhase NewPhase)
{
	if (!CombatManager) return;

	if (NewPhase == ETurnPhase::DrawPhase)
	{
		// 새 턴 시작 시 턴 카운트 텍스트 갱신
		if (Text_TurnCount)
			Text_TurnCount->SetText(FText::FromString(FString::Printf(TEXT("Turn %d"), CombatManager->TurnCount)));
	}
	else if (NewPhase == ETurnPhase::PlayerActionPhase)
	{
		// 플레이어 행동 턴 진입 시 공유 코스트 풀로 리셋
		SharedCost = MaxCost;
		UpdateCostDisplay();
	}
	else if (NewPhase == ETurnPhase::PlayerExecutionPhase)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleMainWidget] PlayerExecutionPhase | HandPanel=%s"),
			HandPanel ? TEXT("valid") : TEXT("NULL"));
		// 턴 종료 → 손패 숨김 및 플레이어 선택 해제
		DeselectCurrentPlayer();
		if (Btn_NextPlayer) Btn_NextPlayer->SetVisibility(ESlateVisibility::Collapsed);
		if (Btn_Back) Btn_Back->SetVisibility(ESlateVisibility::Collapsed);
		if (CombatManager) CombatManager->OnCameraReturnToDefault.Broadcast();
		OnReturnToMainScreen();
	}
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
	// TODO: 사망한 플레이어는 선택되지 않도록 조치 필요
	//       IsAlive() 체크 추가 및 클릭 이벤트 바인딩 해제 (HandleDeath 연동) 예정
	UE_LOG(LogTemp, Warning, TEXT("[BattleMainWidget] Player clicked: %s"), *Unit->GetName());

	// SingleAlly 카드 대기 중: 시전자 본인 제외하고 클릭한 플레이어를 타겟으로 등록
	if (!PendingCardName.IsNone() && PendingCardData.TargetType == ETargetType::SingleAlly)
	{
		if (Unit == SelectedUnit) return; // 시전자 본인은 타겟 불가
		QueueCardAction(PendingCardData, Unit, PendingCardName);
		return;
	}

	// 그 외 대기 상태(SingleEnemy 등)이면 취소 후 플레이어 전환
	if (!PendingCardName.IsNone())
	{
		PendingCardName = NAME_None;
		// 대기 해제 → 캔버스 클릭 통과 복귀
		if (MainCanvas)
			MainCanvas->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		OnPendingCleared();
		// 타겟 대기 해제 알림 (카메라 복귀용)
		if (CombatManager) CombatManager->OnTargetingStateChanged.Broadcast(false);
	}

	// 이전 선택 유닛의 CardUserComponent 바인딩 해제 후 새 유닛 선택
	DeselectCurrentPlayer();
	SelectedUnit = Unit;

	// 카메라에 플레이어 선택 알림 (BattleCameraActor BP가 구독)
	if (CombatManager) CombatManager->OnBattlePlayerSelected.Broadcast(Unit);

	// 플레이어가 2명 이상이면 다음 플레이어 버튼 표시
	if (Btn_NextPlayer && CombatManager && CombatManager->GetSpawnedPlayers().Num() > 1)
		Btn_NextPlayer->SetVisibility(ESlateVisibility::Visible);

	// 플레이어 선택 시 뒤로가기 버튼 표시
	if (Btn_Back)
		Btn_Back->SetVisibility(ESlateVisibility::Visible);

	OnPlayerSelected(Unit);

	// 새 유닛의 CardUserComponent 바인딩 및 현재 손패 즉시 표시
	UCardUserComponent* CardComp = Unit->FindComponentByClass<UCardUserComponent>();
	if (CardComp)
	{
		CardComp->OnHandChanged.AddDynamic(this, &UBattleMainWidget::HandleHandChanged);
		HandleHandChanged(CardComp->GetHand());
		// 플레이어 선택 시에만 등장 애니메이션 재생 (카드 사용 시 갱신과 분리)
		if (HandPanel) HandPanel->PlayShowAnimation();
	}
	else
	{
		OnHandUpdated(TArray<FCardDataRow>());
	}
}

// CardUserComponent::OnHandChanged 수신
// 카드 ID 목록을 CardSubsystem으로 조회해 FCardDataRow 배열로 변환 후 BP에 전달
void UBattleMainWidget::HandleHandChanged(const TArray<FName>& CardNames)
{
	// 선택된 플레이어 없으면 손패 갱신 무시 — 턴 종료 후 드로우 이벤트가 잔여 바인딩으로 도달하는 경우 방지
	if (!SelectedUnit) return;

	// 초과분 정리 중 재방송된 OnHandChanged는 무시 — 재귀 방지
	if (bTrimmingHand) return;

	// 10장 초과 시 뒤에서부터 초과분을 한 장씩 버림 (한 패스로 정리)
	static constexpr int32 MaxHandSize = 10;
	if (CardNames.Num() > MaxHandSize)
	{
		UCardUserComponent* CardComp = SelectedUnit->FindComponentByClass<UCardUserComponent>();
		if (CardComp)
		{
			// RemoveFromHand이 OnHandChanged를 재방송하므로 가드를 세워 재진입을 막고 한 곳에서 정리
			bTrimmingHand = true;
			int32 Overflow = CardNames.Num() - MaxHandSize;
			for (int32 i = CardNames.Num() - 1; i >= 0 && Overflow > 0; --i)
			{
				const FName OverflowCard = CardNames[i];
				// 제거 실패 시 카운트가 줄지 않아 무한 루프가 되므로 즉시 중단
				if (!CardComp->RemoveFromHand(OverflowCard))
					break;
				CardComp->DiscardSpecificCard(OverflowCard);
				--Overflow;
			}
			bTrimmingHand = false;

			// 정리된 손패를 다시 표시
			HandleHandChanged(CardComp->GetHand());
		}
		return;
	}

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

	// HandPanel이 연결되어 있으면 C++로 직접 ShowHand 호출, 없으면 BP 이벤트로 폴백
	if (HandPanel)
	{
		HandPanel->ShowHand(Cards);

		// 덱 카운트 갱신
		UCardUserComponent* CardComp = SelectedUnit->FindComponentByClass<UCardUserComponent>();
		if (CardComp)
			HandPanel->UpdateDeckCounts(CardComp->GetDrawPileCount(), CardComp->GetDiscardPileCount());
	}
	else
		OnHandUpdated(Cards);
}

// HandPanel::OnCardSelected 수신
// WBP_Card가 broadcast하는 CardID 필드값을 Row Name으로 변환 후 코스트 검증 및 큐 등록
void UBattleMainWidget::HandleCardClicked(FName CardName, UCardWidget* ClickedCard)
{
	// OnHandChanged 콜백 체인 등에서 재진입 시 무시 — 이중 바인딩·이중 호출 방어
	if (bIsProcessingCard) return;
	ON_SCOPE_EXIT { bIsProcessingCard = false; };
	bIsProcessingCard = true;

	if (!SelectedUnit || !CombatManager) return;

	UCardSubsystem* CS = GetGameInstance()
		? GetGameInstance()->GetSubsystem<UCardSubsystem>()
		: nullptr;
	if (!CS) return;

	// CardName은 WBP_Card의 CardID 필드값 — Row Name과 다를 수 있으므로 역조회
	FName RowName = CS->GetRowNameByCardID(CardName);
	if (RowName.IsNone()) RowName = CardName; // Row Name == CardID인 경우 폴백

	// 같은 카드 재선택 시 선택 해제 (Row Name 기준 비교)
	if (PendingCardName == RowName)
	{
		CancelPendingCard();
		return;
	}

	const FCardDataRow* Row = CS->GetCard(RowName);
	if (!Row) return;

	// 코스트 부족 시 — SetCardPendingDirect로 걸린 pending 상태 해제 후 무시
	if (SharedCost < Row->Cost)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleMainWidget] Not enough cost for %s (need %d, have %d)"),
			*CardName.ToString(), Row->Cost, SharedCost);
		if (HandPanel) HandPanel->ClearCardPending();
		return;
	}

	// Self / AllAllies / AllEnemies 타겟은 적 선택 없이 즉시 큐에 등록
	if (Row->TargetType == ETargetType::Self       ||
		Row->TargetType == ETargetType::AllAllies  ||
		Row->TargetType == ETargetType::AllEnemies ||
		Row->TargetType == ETargetType::Single_Team)
	{
		QueueCardAction(*Row, nullptr, RowName);
		return;
	}

	// SingleEnemy / SingleAlly: 적(아군) 선택 대기 상태로 전환 — Row Name 저장
	PendingCardName = RowName;
	PendingCardData = *Row;
	if (MainCanvas)
		MainCanvas->SetVisibility(ESlateVisibility::Visible);
	if (HandPanel) HandPanel->SetTargetingMode(true);
	OnCardPending(CardName); // UI에는 CardID 전달 (표시용)
	// SingleAlly: 아군 타겟팅 카메라 / SingleEnemy: 적 타겟팅 카메라
	if (CombatManager)
	{
		if (Row->TargetType == ETargetType::SingleAlly)
			CombatManager->OnAllyTargetingStateChanged.Broadcast(true);
		else
			CombatManager->OnTargetingStateChanged.Broadcast(true);
	}
	UE_LOG(LogTemp, Log, TEXT("[BattleMainWidget] Card pending: %s (RowName: %s) — waiting for target"),
		*CardName.ToString(), *RowName.ToString());
}

// 적 유닛 클릭 시 호출
// 클릭 수신 여부를 항상 로그로 출력, PendingCard가 있으면 타겟으로 큐에 등록
void UBattleMainWidget::HandleEnemyClicked(AUnit* Enemy)
{
	if (!Enemy) return;

	// 적 클릭 수신 확인 로그 (PendingCard 여부와 무관하게 항상 출력)
	UE_LOG(LogTemp, Warning, TEXT("[BattleMainWidget] Enemy clicked: %s | PendingCard: %s"),
		*Enemy->GetName(),
		PendingCardName.IsNone() ? TEXT("none") : *PendingCardName.ToString());

	if (PendingCardName.IsNone()) return;

	// PendingCardName은 Row Name으로 저장되어 있음 — QueueCardAction에 함께 전달
	QueueCardAction(PendingCardData, Enemy, PendingCardName);
}

// SpawnedEnemies 각각의 OnUnitClicked에 바인딩
void UBattleMainWidget::BindEnemyClickEvents()
{
	if (!CombatManager) return;

	int32 BoundCount = 0;
	for (AUnit* Unit : CombatManager->GetSpawnedEnemies())
	{
		if (Unit)
		{
			Unit->OnUnitClicked.AddDynamic(this, &UBattleMainWidget::HandleEnemyClicked);
			BoundCount++;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[BattleMainWidget] Enemy click events bound: %d enemies"), BoundCount);
}

// 카드+타겟이 확정됐을 때 호출
// Hand에서 카드를 제거하고 CombatManager 큐에 등록한 뒤 코스트를 차감
// CardRowName — RemoveFromHand와 DiscardSpecificCard에 사용할 Row Name (없으면 CardID 폴백)
void UBattleMainWidget::QueueCardAction(const FCardDataRow& CardData, AUnit* TargetOverride, FName CardRowName)
{
	if (!SelectedUnit || !CombatManager) return;

	const int32 CasterIndex = CombatManager->GetSpawnedPlayers().IndexOfByKey(SelectedUnit);
	if (CasterIndex == INDEX_NONE) return;

	UCardUserComponent* CardComp = SelectedUnit->FindComponentByClass<UCardUserComponent>();
	if (!CardComp) return;

	// RemoveFromHand에 Row Name 우선 사용 (CardID != Row Name인 DataTable 구성 대응)
	const FName RemoveKey = CardRowName.IsNone() ? CardData.CardID : CardRowName;

	// Hand에서 카드 제거 + OnHandChanged 브로드캐스트 → 손패 UI 즉시 갱신
	if (CardComp->RemoveFromHand(RemoveKey))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleMainWidget] Card removed from hand: %s | Hand count: %d"),
			*RemoveKey.ToString(), CardComp->GetHandCount());

		SharedCost -= CardData.Cost;
		UpdateCostDisplay();

		// 카드 효과 즉시 실행
		CombatManager->ExecuteCard(CardData, CasterIndex, TargetOverride);

		// DrawCount가 있으면 즉시 드로우 (드로우 카드 효과)
		if (CardData.DrawCount > 0)
			CardComp->DrawCards(CardData.DrawCount);

		// 카드를 DiscardPile/ExhaustPile로 이동
		CardComp->DiscardSpecificCard(RemoveKey);

		// DiscardSpecificCard 이후 덱 카운트 UI 갱신
		if (HandPanel)
			HandPanel->UpdateDeckCounts(CardComp->GetDrawPileCount(), CardComp->GetDiscardPileCount());

		// 이번 턴 사용 기록 저장 (순서 추적용)
		CombatManager->QueuePlayerAction(CardData, CasterIndex, CardRowName, TargetOverride);

		UE_LOG(LogTemp, Warning, TEXT("[BattleMainWidget] Card executed: %s | Cost left: %d"),
			*CardData.CardID.ToString(), SharedCost);
	}

	// 선택 대기 상태 초기화 — 타겟 대기 중이었으면 타겟팅 모드 해제 및 카메라 복귀 브로드캐스트까지 일괄 처리
	// (대기 중이 아니었으면 CancelPendingCard 내부에서 조기 반환)
	CancelPendingCard();
}

// Btn_EndTurn 클릭 → PlayerExecutionPhase 진입 후 큐 실행
// 대기 중인 카드가 있으면 먼저 취소하여 다음 턴에 OnPendingCleared가 오발되는 것을 방지
void UBattleMainWidget::HandleEndTurnClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("[BattleMainWidget] HandleEndTurnClicked called"));
	CancelPendingCard();

	// 턴 종료 시 모든 플레이어의 손패를 버림 파일로 이동
	if (CombatManager)
	{
		for (AUnit* Player : CombatManager->GetSpawnedPlayers())
		{
			if (!Player) continue;
			if (UCardUserComponent* CardComp = Player->FindComponentByClass<UCardUserComponent>())
				CardComp->DiscardHand();
		}
	}

	// 버튼 즉시 숨김
	if (Btn_NextPlayer) Btn_NextPlayer->SetVisibility(ESlateVisibility::Collapsed);
	if (Btn_Back) Btn_Back->SetVisibility(ESlateVisibility::Collapsed);

	// 손패 먼저 숨긴 후 턴 종료
	if (HandPanel)
		HandPanel->PlayHideAnimation();
	else
		OnHideHand();

	if (CombatManager)
		CombatManager->EndPlayerActionPhase();
}

// Btn_NextPlayer 클릭 → 다음 살아있는 플레이어로 선택 전환
void UBattleMainWidget::HandleNextPlayerClicked()
{
	SelectNextPlayer();
}

// Btn_Back 클릭 → 현재 상태에 따라 한 단계씩 뒤로 이동
// 타겟 지정 중: 타겟만 취소 (플레이어 슬롯으로 카메라 복귀, 선택 유지)
// 플레이어 선택 중: 선택 해제 후 Default 위치로 복귀
void UBattleMainWidget::HandleBackClicked()
{
	// 카메라 이동 중 입력 차단
	if (CombatManager && CombatManager->bCameraMoving) return;

	// ── 단계 1: 타겟 지정 중이면 취소만 하고 종료 ────────────────
	// CancelPendingCard → OnTargetingStateChanged(false) → 카메라 Reverse(플레이어 슬롯 복귀)
	if (!PendingCardName.IsNone())
	{
		CancelPendingCard();
		return;
	}

	// ── 단계 2: 플레이어 선택 상태 → 완전 복귀 ──────────────────
	DeselectCurrentPlayer();

	// 손패 역모션 재생 — BP PlayHideAnimation 완료 시 ClearHand() + Hidden 처리
	if (HandPanel)
		HandPanel->PlayHideAnimation();
	else
		OnHideHand();

	if (Btn_NextPlayer)
		Btn_NextPlayer->SetVisibility(ESlateVisibility::Collapsed);

	// 뒤로가기 버튼 숨김 — Default 위치에서는 더 이상 누를 수 없음
	if (Btn_Back)
		Btn_Back->SetVisibility(ESlateVisibility::Collapsed);

	// 카메라 Default 위치 복귀
	if (CombatManager) CombatManager->OnCameraReturnToDefault.Broadcast();

	// BP에 메인 플레이 화면 복귀 알림
	OnReturnToMainScreen();
}

// 타겟 대기 중 빈 영역 또는 유효하지 않은 위치 클릭 시 대기 카드 취소
// 유닛이 클릭된 경우에는 기존 HandleEnemyClicked / HandlePlayerClicked 흐름에 위임
FReply UBattleMainWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 대기 카드 없거나 좌클릭이 아니면 기본 처리
	if (PendingCardName.IsNone() || InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	// 클릭 위치에 살아있는 유닛이 있으면 NotifyActorOnClicked 흐름(HandleEnemyClicked 등)에 위임
	// ECC_Visibility 우선, 없으면 ECC_Pawn 채널로 재시도 (유닛 콜리전 설정에 따라 다를 수 있음)
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		auto TryFindUnit = [&](ECollisionChannel Channel) -> AUnit*
		{
			FHitResult Hit;
			if (PC->GetHitResultUnderCursor(Channel, false, Hit))
				return Cast<AUnit>(Hit.GetActor());
			return nullptr;
		};

		AUnit* HitUnit = TryFindUnit(ECC_Visibility);
		if (!HitUnit)
			HitUnit = TryFindUnit(ECC_Pawn);

		UE_LOG(LogTemp, Warning, TEXT("[BattleMainWidget] MouseDown | PendingCard: %s | HitUnit: %s | Alive: %s"),
			*PendingCardName.ToString(),
			HitUnit ? *HitUnit->GetName() : TEXT("none"),
			(HitUnit && HitUnit->IsAlive()) ? TEXT("true") : TEXT("false"));

		// FReply::Unhandled() 만으로는 3D 액터의 OnUnitClicked까지 전파되지 않으므로 직접 호출
		if (HitUnit && HitUnit->IsAlive())
		{
			if (PendingCardData.TargetType == ETargetType::SingleEnemy)
				HandleEnemyClicked(HitUnit);
			else if (PendingCardData.TargetType == ETargetType::SingleAlly)
				HandlePlayerClicked(HitUnit);
			return FReply::Handled();
		}
	}

	// 유효한 유닛 없는 곳 클릭 → 대기 취소
	CancelPendingCard();
	return FReply::Unhandled();
}

// 현재 대기 중인 카드 선택을 취소하고 BP에 알림
void UBattleMainWidget::CancelPendingCard()
{
	if (PendingCardName.IsNone()) return;

	// 타입에 따라 올바른 카메라 복귀 델리게이트 브로드캐스트
	if (CombatManager)
	{
		if (PendingCardData.TargetType == ETargetType::SingleAlly)
			CombatManager->OnAllyTargetingStateChanged.Broadcast(false);
		else
			CombatManager->OnTargetingStateChanged.Broadcast(false);
	}

	PendingCardName = NAME_None;
	if (MainCanvas)
		MainCanvas->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (HandPanel) HandPanel->SetTargetingMode(false);
	OnPendingCleared();
}

// SpawnedPlayers에서 현재 선택 다음 인덱스의 살아있는 플레이어로 전환
void UBattleMainWidget::SelectNextPlayer()
{
	if (!CombatManager) return;

	const TArray<AUnit*>& Players = CombatManager->GetSpawnedPlayers();
	if (Players.Num() <= 1) return;

	const int32 CurrentIndex = Players.IndexOfByKey(SelectedUnit);
	// 현재 인덱스 다음부터 순환하며 살아있는 플레이어 탐색
	for (int32 i = 1; i < Players.Num(); i++)
	{
		const int32 NextIndex = (CurrentIndex + i) % Players.Num();
		if (Players.IsValidIndex(NextIndex) && Players[NextIndex] && Players[NextIndex]->IsAlive())
		{
			HandlePlayerClicked(Players[NextIndex]);
			return;
		}
	}
}

// 남은 코스트를 Text_Cost에 반영하고 OnCostChanged BP 이벤트 호출
void UBattleMainWidget::UpdateCostDisplay()
{
	if (Text_Cost)
		Text_Cost->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), SharedCost, MaxCost)));
	OnCostChanged(SharedCost, MaxCost);
}

// 현재 선택 플레이어의 OnHandChanged 바인딩을 해제하고 SelectedUnit을 초기화
void UBattleMainWidget::DeselectCurrentPlayer()
{
	if (!SelectedUnit) return;

	UCardUserComponent* CardComp = SelectedUnit->FindComponentByClass<UCardUserComponent>();
	if (CardComp)
		CardComp->OnHandChanged.RemoveDynamic(this, &UBattleMainWidget::HandleHandChanged);

	SelectedUnit = nullptr;
}
