#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardDataTypes.h"
#include "CardWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "BattleMainWidget.generated.h"

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
	class UCanvasPanel* MainCanvas;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCardWidget> NewCard;

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

	UFUNCTION()
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
};
