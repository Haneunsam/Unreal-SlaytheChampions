// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CardDataTypes.h"
#include "Engine/Texture2D.h"
#include "CardStruct.generated.h"

// 손패에서 사용하는 런타임 카드 인스턴스.
// FCardDataRow(DataTable 정적 정의)와 필드를 맞춰두어 나중에 변환이 쉽도록 한다.
USTRUCT(BlueprintType)
struct FCardStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CardName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardType CardType = ECardType::Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETargetType TargetType = ETargetType::SingleEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Cost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Damage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Block = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HealAmount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DrawCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CardImage = nullptr;
};