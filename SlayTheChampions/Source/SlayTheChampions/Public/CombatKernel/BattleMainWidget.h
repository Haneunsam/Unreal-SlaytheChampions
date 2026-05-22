#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Card/CardDataTypes.h"
#include "Card/CardWidget.h"
#include "Card/HandComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "CombatKernel/CombatManager.h"
#include "GameManagers/MouseManager.h"
#include "BattleMainWidget.generated.h"

class ACombatManager;
class UTextBlock;

/**
 * FWidgetCardsStruct
 * 손패의 카드 위젯 하나를 관리하는 데이터 묶음.
 * CardWidget·Canvas·CardSlot을 함께 보관해 위치 계산 시 참조한다.
 */
USTRUCT(BlueprintType)
struct FWidgetCardsStruct
{
	GENERATED_BODY()

	// 실제 카드 위젯 인스턴스
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCardWidget* CardWidget = nullptr;

	// 카드가 배치된 캔버스 패널
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCanvasPanel* Canvas = nullptr;

	// 위치·크기 제어용 슬롯
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCanvasPanelSlot* CardSlot = nullptr;
};

/**
 * UBattleMainWidget
 * 전투 화면 최상위 위젯. 손패 카드 배치, 코스트 관리, 턴 텍스트 갱신을 담당한다.
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

	// 손패 카드가 배치될 캔버스 (WBP_BattleMainWidget 디자이너에서 위치 지정)
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* HandCanvas;

	// 생성할 카드 위젯 클래스 (에디터에서 지정)
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCardWidget> NewCard;

	// 카드 효과 실행을 위한 CombatManager 참조 (NativeConstruct에서 레벨 자동 탐색)
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	ACombatManager* CombatManager;

	// 손패 관리 컴포넌트 (이 위젯이 소유)
	UPROPERTY(BlueprintReadOnly, Category = "Card|Hand")
	UHandComponent* Hand;

	// 현재 손패의 카드 위젯 목록
	UPROPERTY()
	TArray<FWidgetCardsStruct> WidgetCards;

	// 손패 카드의 하단 여백 (픽셀)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CardPosition")
	float BottomMargin = 30.f;

	// 카드 위젯 높이 (픽셀)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CardPosition")
	float CardHeight = 300.f;

	// 카드 위젯 너비 (픽셀)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CardPosition")
	float CardWidth = 200.f;

	// 이번 턴 남은 공유 코스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 SharedCost = 3;

	// 턴당 최대 코스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 MaxCost = 3;

	// 손패에 카드를 추가하고 OrganizeCards로 위치를 재배치
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AddCard(const FCardDataRow& InCardData);

	// 손패 카드들의 X 위치를 고르게 재배치
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OrganizeCards(float OffsetX);

	// 화면 크기 기반으로 하단 패딩과 중심 X를 계산
	UFUNCTION()
	void FN_GetScreenInfo(float InBottomMargin, float InCardHeight, float& OutBottomPadding, float& OutCenterOfScreen);

	// 카드 실행 완료 시 호출 — BP에서 연출 추가 가능 (BlueprintNativeEvent)
	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void OnCardExecuted(const FCardDataRow& Card);
	void OnCardExecuted_Implementation(const FCardDataRow& Card);

	// 코스트 변경 시 BP에서 UI 갱신 (BlueprintImplementableEvent)
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnCostChanged(int32 Current, int32 Max);

	// 카드 클릭 시 BP 또는 외부에서 직접 호출
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void HandleCardClicked(UCardWidget* Widget, const FCardDataRow& Card);

protected:
	// 현재 턴 수 표시 텍스트 (BindWidget)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_TurnCount;

	virtual void NativeConstruct() override;

private:
	// OnPhaseChanged 델리게이트 수신 → 턴 텍스트 및 코스트 갱신
	UFUNCTION()
	void OnPhaseChanged(ETurnPhase NewPhase);
};
