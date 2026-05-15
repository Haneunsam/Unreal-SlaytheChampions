// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatKernel/CardWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "WidgetCardsStruct.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FWidgetCardsStruct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCardWidget* CardWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCanvasPanel* Canvas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCanvasPanelSlot* CardSlot;
};
