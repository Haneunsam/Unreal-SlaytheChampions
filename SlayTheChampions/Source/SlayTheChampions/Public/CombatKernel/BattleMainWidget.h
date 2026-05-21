#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardDataTypes.h"
#include "CardWidget.h"
#include "HandComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "CombatKernel/CombatManager.h"
#include "GameManagers/MouseManager.h"
#include "BattleMainWidget.generated.h"

class ACombatManager;
class UTextBlock;

USTRUCT(BlueprintType)
struct FWidgetCardsStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCardWidget* CardWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCanvasPanel* Canvas = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCanvasPanelSlot* CardSlot = nullptr;
};

UCLASS()
class SLAYTHECHAMPIONS_API UBattleMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MainCanvas;

	// 손패 카드가 배치될 캔버스 (WBP_BattleMainWidget 디자이너에서 위치 지정)
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* HandCanvas;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCardWidget> NewCard;

	// 카드 효과 실행을 위한 CombatManager 참조 (NativeConstruct에서 레벨 자동 탐색)
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	ACombatManager* CombatManager;

	// 손패 관리 컴포넌트 (이 위젯이 소유)
	UPROPERTY(BlueprintReadOnly, Category = "Card|Hand")
	UHandComponent* Hand;

	UPROPERTY()
	TArray<FWidgetCardsStruct> WidgetCards;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CardPosition")
	float BottomMargin = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CardPosition")
	float CardHeight = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CardPosition")
	float CardWidth = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 SharedCost = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 MaxCost = 3;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AddCard(const FCardDataRow& InCardData);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OrganizeCards(float OffsetX);

	UFUNCTION()
	void FN_GetScreenInfo(float InBottomMargin, float InCardHeight, float& OutBottomPadding, float& OutCenterOfScreen);

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void OnCardExecuted(const FCardDataRow& Card);
	void OnCardExecuted_Implementation(const FCardDataRow& Card);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnCostChanged(int32 Current, int32 Max);

	// 카드 클릭 시 BP 또는 외부에서 직접 호출
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void HandleCardClicked(UCardWidget* Widget, const FCardDataRow& Card);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_TurnCount;

	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnPhaseChanged(ETurnPhase NewPhase);
};
