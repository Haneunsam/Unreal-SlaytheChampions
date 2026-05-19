// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CombatKernel/CardStruct.h"
#include "CardWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCardClicked, UCardWidget*, Widget, const FCardStruct&, CardData);

UCLASS()
class SLAYTHECHAMPIONS_API UCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Card")
	FCardStruct CardData;

	UFUNCTION(BlueprintCallable, Category = "Card")
	void SetCardData(const FCardStruct& InData);

	UPROPERTY(BlueprintAssignable, Category = "Card")
	FOnCardClicked OnCardClicked;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
