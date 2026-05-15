// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CombatKernel/WidgetCardsStruct.h"
#include "CombatKernel/CardWidget.h"
#include "CombatKernel/CardPlayerController.h"
#include "BattleMainWidget.generated.h"

UCLASS()
class SLAYTHECHAMPIONS_API UBattleMainWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	bool bDoOnce = false;
	FTimerHandle OrganizeTimerHandle;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controller")
	ACardPlayerController* CardPlayerController;

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* MainCanvas;

	UPROPERTY(meta = (BindWidget))
	class UButton* AddNewCardButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AddNewText;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CardPosition")
	float CenterOfScreen = 300.f;

	// 파티 공용 코스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 SharedCost = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 MaxCost = 3;

	// 파티원 0
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Party")
	int32 Party0_HP = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Party")
	int32 Party0_MaxHP = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Party")
	int32 Party0_Block = 0;

	// 파티원 1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Party")
	int32 Party1_HP = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Party")
	int32 Party1_MaxHP = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Party")
	int32 Party1_Block = 0;

	// 현재 선택된 파티원 인덱스 (0 또는 1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Party")
	int32 ActivePartyIndex = 0;

	// 적
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Enemy")
	int32 EnemyHP = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Enemy")
	int32 EnemyMaxHP = 30;

	UFUNCTION()
	void API_AddNewPlayerCard();

	// 카드 데이터를 지정해서 손패에 추가한다
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AddCard(const FCardStruct& InCardData);

	UFUNCTION()
	void OnOrganizeDelay();

	UFUNCTION()
	void OrganizeCards(float OffsetX);

	UFUNCTION()
	void FN_GetScreenInfo(float InBottomMargin, float InCardHeight, float& OutBottomPadding, float& OutCenterOfScreen);

	// 파티원 선택 전환 (0 또는 1)
	UFUNCTION(BlueprintCallable, Category = "Combat|Party")
	void SelectPartyMember(int32 Index);

	// 카드 효과 적용 (Attack: 적 HP 감소, Defence: 선택 파티원 Block 증가)
	// BP에서 오버라이드해서 UI 갱신 추가 가능
	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void OnCardExecuted(const FCardStruct& Card);
	void OnCardExecuted_Implementation(const FCardStruct& Card);

	// 코스트가 바뀔 때 BP에서 UI 갱신
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnCostChanged(int32 Current, int32 Max);

	// 스탯이 바뀔 때 BP에서 UI 갱신
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnPartyStatsChanged(int32 Index, int32 HP, int32 MaxHP, int32 Block);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnEnemyStatsChanged(int32 HP, int32 MaxHP);

private:
	UFUNCTION()
	void HandleCardClicked(UCardWidget* Widget, const FCardStruct& Card);
};
