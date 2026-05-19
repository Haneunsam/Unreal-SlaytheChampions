// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatKernel/CardWidget.h"

void UCardWidget::SetCardData(const FCardStruct& InData)
{
	CardData = InData;
}

FReply UCardWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnCardClicked.Broadcast(this, CardData);
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
