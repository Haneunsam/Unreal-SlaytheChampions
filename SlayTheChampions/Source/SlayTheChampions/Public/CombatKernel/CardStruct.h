// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatKernel/AttackTypeEnum.h"
#include "Engine/Texture2D.h"
#include "CardStruct.generated.h"

USTRUCT(BlueprintType)
struct FCardStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttackType Type = EAttackType::Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CardCost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Discription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CardImage = nullptr;
};