#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Card/CardDataTypes.h"
#include "Components/CanvasPanel.h"
#include "CombatKernel/CombatManager.h"
#include "GameManagers/MouseManager.h"
#include "BattleMainWidget.generated.h"

class ACombatManager;
class UTextBlock;
class UButton;
class AUnit;
class UHandWidget;
class UActionHistoryWidget;

/**
 * UBattleMainWidget
 * 전투 화면 최상위 위젯. 코스트 관리, 턴 텍스트 갱신을 담당한다.
 * NativeConstruct에서 레벨의 CombatManager를 자동으로 탐색해 바인딩한다.
 */
UCLASS()
class SLAYTHECHAMPIONS_API UBattleMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 루트 캔버스 (BindWidget)
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MainCanvas;

	// 카드 효과 실행을 위한 CombatManager 참조 (NativeConstruct에서 레벨 자동 탐색)
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	ACombatManager* CombatManager;

	// 현재 선택된 플레이어 유닛
	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	AUnit* SelectedUnit = nullptr;

	// 이번 턴 남은 공유 코스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 SharedCost = 3;

	// 턴당 최대 코스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 MaxCost = 3;

	// 코스트 변경 시 BP에서 UI 갱신 (BlueprintImplementableEvent)
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnCostChanged(int32 Current, int32 Max);

	// 플레이어 유닛 선택 시 BP에 알림 — 선택 강조 연출 등에 사용
	UFUNCTION(BlueprintImplementableEvent, Category = "Selection")
	void OnPlayerSelected(AUnit* Unit);

	// 뒤로가기 클릭으로 메인 플레이 화면 복귀 시 BP에 알림 — 손패 패널 숨김 등 UI 복귀 처리용
	UFUNCTION(BlueprintImplementableEvent, Category = "Selection")
	void OnReturnToMainScreen();

	// 카드 선택 대기 진입 시 BP에 알림 (하이라이트 연출용)
	UFUNCTION(BlueprintImplementableEvent, Category = "Selection")
	void OnCardPending(FName CardName);

	// 카드 선택 대기 해제 시 BP에 알림 (하이라이트 제거용)
	UFUNCTION(BlueprintImplementableEvent, Category = "Selection")
	void OnPendingCleared();

	// 손패 목록 변경 시 HandWidget에 직접 전달
	// BP에서 HandPanel(UHandWidget)을 연결해두면 자동으로 ShowHand 호출
	UFUNCTION(BlueprintImplementableEvent, Category = "Selection")
	void OnHandUpdated(const TArray<FCardDataRow>& Cards);

	// 손패 숨김 요청 — HandPanel C++ 바인딩 실패 시 BP 폴백
	// WBP_BattleMainWidget에서 구현: HandPanel 위젯 참조로 PlayHideAnimation 호출
	UFUNCTION(BlueprintImplementableEvent, Category = "Selection")
	void OnHideHand();

	// 플레이어 턴 종료 버튼 클릭 시 BP 트리거 (연출·사운드용)
	UFUNCTION(BlueprintImplementableEvent, Category = "Turn")
	void OnPlayerTurnEnd();

	// 대기 카드 취소 — BP의 오버레이 버튼, 우클릭 등에서 직접 호출 가능
	UFUNCTION(BlueprintCallable, Category = "Selection")
	void CancelPendingCard();

	// 현재 타겟 대기 상태 여부 — BP에서 화살표 오버레이 가시성 제어에 사용
	UFUNCTION(BlueprintPure, Category = "Selection")
	bool IsCardPending() const { return !PendingCardName.IsNone(); }

	// 손패 패널 참조 (WBP에서 BindWidget 또는 BP에서 직접 할당)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional), Category = "Hand")
	UHandWidget* HandPanel;

	// 이번 턴 사용 카드 히스토리 패널 (WBP에서 ActionHistoryPanel 이름으로 추가)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional), Category = "History")
	UActionHistoryWidget* ActionHistoryPanel;

protected:
	// 유닛 외 영역 클릭 시 대기 카드 취소 처리
	// MainCanvas가 ESlateVisibility::Visible일 때만 동작
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	// 현재 턴 수 표시 텍스트 (BindWidget)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_TurnCount;

	// 남은 코스트 표시 텍스트 (BindWidgetOptional — WBP에서 Text_Cost 이름으로 추가)
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* Text_Cost;

	// 턴 종료 버튼 — 클릭 시 손패 정리 후 EndPlayerActionPhase 호출
	UPROPERTY(meta = (BindWidgetOptional))
	UButton* Btn_EndTurn;

	// 다음 플레이어 버튼 — 클릭 시 다음 플레이어로 선택 전환 (핸드 표시 시 노출)
	UPROPERTY(meta = (BindWidgetOptional))
	UButton* Btn_NextPlayer;

	// 뒤로가기 버튼 — 플레이어 선택 해제 후 메인 플레이 화면으로 복귀
	UPROPERTY(meta = (BindWidgetOptional))
	UButton* Btn_Back;

	virtual void NativeConstruct() override;

private:
	// OnPhaseChanged 델리게이트 수신 → 턴 텍스트 갱신
	UFUNCTION()
	void OnPhaseChanged(ETurnPhase NewPhase);

	// SpawnedPlayers의 OnUnitClicked에 일괄 바인딩
	void BindPlayerClickEvents();

	// 플레이어 유닛 클릭 시 호출 — SelectedUnit 갱신 및 손패 표시
	UFUNCTION()
	void HandlePlayerClicked(AUnit* Unit);

	// HandComponent::OnHandChanged 수신 → 카드 데이터 조회 후 OnHandUpdated 호출
	UFUNCTION()
	void HandleHandChanged(const TArray<FName>& CardNames);

	// HandPanel::OnCardSelected 수신 → 코스트 검증 후 선택 대기 상태로 전환
	UFUNCTION()
	void HandleCardClicked(FName CardName, UCardWidget* ClickedCard);

	// 적 유닛 클릭 시 호출 — PendingCard가 있으면 타겟으로 큐에 등록
	UFUNCTION()
	void HandleEnemyClicked(AUnit* Enemy);

	// SpawnedEnemies 각각의 OnUnitClicked에 바인딩
	void BindEnemyClickEvents();

	// 카드+타겟 확정 후 큐 등록 + Hand 제거 + 코스트 차감을 처리하는 헬퍼
	// CardRowName — RemoveFromHand/QueuePlayerAction에 사용할 Row Name (NAME_None이면 CardData.CardID 사용)
	void QueueCardAction(const FCardDataRow& CardData, AUnit* TargetOverride, FName CardRowName = NAME_None);

	// Btn_EndTurn 클릭 → CombatManager::EndPlayerActionPhase 호출
	UFUNCTION()
	void HandleEndTurnClicked();

	// Btn_NextPlayer 클릭 → 다음 살아있는 플레이어로 선택 전환
	UFUNCTION()
	void HandleNextPlayerClicked();

	// Btn_Back 클릭 → 선택 해제 후 메인 플레이 화면으로 복귀
	UFUNCTION()
	void HandleBackClicked();

	// 다음 플레이어 인덱스를 찾아 HandlePlayerClicked 호출
	void SelectNextPlayer();

	// 남은 코스트를 Text_Cost에 반영하고 OnCostChanged BP 이벤트 호출 — 코스트 표시 일원화
	void UpdateCostDisplay();

	// 현재 선택 플레이어의 OnHandChanged 바인딩 해제 후 SelectedUnit 초기화 — 선택 해제 일원화
	void DeselectCurrentPlayer();

	// 적 선택을 기다리는 카드 이름 (IsNone이면 미선택 상태)
	FName PendingCardName;

	// PendingCardName에 대응하는 카드 전체 데이터
	FCardDataRow PendingCardData;

	// HandleCardClicked 재진입 방지 플래그 — OnHandChanged 콜백 체인에서 중복 호출 차단
	bool bIsProcessingCard = false;

	// 손패 초과분 정리 중 플래그 — RemoveFromHand의 OnHandChanged 재방송으로 인한 재귀 차단
	bool bTrimmingHand = false;
};
