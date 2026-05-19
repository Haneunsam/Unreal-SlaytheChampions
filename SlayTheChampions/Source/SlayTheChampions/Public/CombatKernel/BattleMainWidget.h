#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CombatKernel/WidgetCardsStruct.h"
#include "CombatKernel/CardWidget.h"
#include "BattleMainWidget.generated.h"

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
	void AddCard(const FCardStruct& InCardData);

	UFUNCTION()
	void OrganizeCards(float OffsetX);

	UFUNCTION()
	void FN_GetScreenInfo(float InBottomMargin, float InCardHeight, float& OutBottomPadding, float& OutCenterOfScreen);

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void OnCardExecuted(const FCardStruct& Card);
	void OnCardExecuted_Implementation(const FCardStruct& Card);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnCostChanged(int32 Current, int32 Max);

private:
	UFUNCTION()
	void HandleCardClicked(UCardWidget* Widget, const FCardStruct& Card);
};
