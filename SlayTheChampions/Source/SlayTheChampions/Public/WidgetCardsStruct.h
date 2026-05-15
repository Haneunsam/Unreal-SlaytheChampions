// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetCardsStruct.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FCardsStruct 
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCardWidget* CardWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCanvasPanel* Canvas;
};
