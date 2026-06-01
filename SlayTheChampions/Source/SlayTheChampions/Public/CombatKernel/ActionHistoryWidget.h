#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Card/CardDataTypes.h"
#include "CombatKernel/CombatManager.h"
#include "ActionHistoryWidget.generated.h"

class UVerticalBox;

/**
 * UActionHistoryWidget
 * 이번 턴에 플레이어가 사용한 카드를 순서대로 표시하는 위젯.
 * WBP_ActionHistory가 이 클래스를 부모로 사용한다.
 *
 * CombatManager를 직접 탐색·구독하고,
 * OnActionExecuted 수신 시 AddEntry를 브로드캐스트,
 * OnPhaseChanged(DrawPhase) 수신 시 ClearHistory를 브로드캐스트한다.
 */
UCLASS()
class SLAYTHECHAMPIONS_API UActionHistoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// ── 공개 API ─────────────────────────────────────────────────────────

	// CombatManager 수동 설정 — NativeConstruct 자동 탐색 실패 시 BP에서 호출
	UFUNCTION(BlueprintCallable, Category = "History")
	void SetCombatManager(ACombatManager* Manager);

	// ── BP 구현부 ─────────────────────────────────────────────────────────

	// 새 카드 항목을 히스토리 목록에 추가 — BP에서 라벨/아이콘 위젯 생성 후 HistoryBox에 추가
	UFUNCTION(BlueprintImplementableEvent, Category = "History")
	void OnAddEntry(const FCardDataRow& Card, int32 CasterIndex);

	// 히스토리 전체 초기화 — BP에서 HistoryBox를 ClearChildren 처리
	UFUNCTION(BlueprintImplementableEvent, Category = "History")
	void OnClearHistory();

protected:
	// NativeConstruct에서 레벨의 CombatManager를 자동 탐색해 델리게이트를 바인딩
	virtual void NativeConstruct() override;

	// NativeDestruct에서 구독 해제
	virtual void NativeDestruct() override;

	// 히스토리 항목 표시용 컨테이너 — WBP에서 이름 'HistoryBox' 일치 필요
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UVerticalBox* HistoryBox;

private:
	// 바인딩된 CombatManager 참조
	UPROPERTY()
	ACombatManager* CombatManager = nullptr;

	// CombatManager::OnActionExecuted 수신 → OnAddEntry 브로드캐스트
	UFUNCTION()
	void HandleActionExecuted(FCardDataRow Card, int32 CasterIndex);

	// CombatManager::OnPhaseChanged 수신 → DrawPhase일 때 OnClearHistory 브로드캐스트
	UFUNCTION()
	void HandlePhaseChanged(ETurnPhase NewPhase);
};
