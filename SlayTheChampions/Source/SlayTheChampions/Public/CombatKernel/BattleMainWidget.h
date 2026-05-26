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
class AUnit;

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

	// 손패 목록 변경 시 BP에 카드 데이터 전달 — BP에서 카드 위젯 생성/갱신
	UFUNCTION(BlueprintImplementableEvent, Category = "Selection")
	void OnHandUpdated(const TArray<FCardDataRow>& Cards);

protected:
	// 현재 턴 수 표시 텍스트 (BindWidget)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_TurnCount;

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
};
