// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Relic/RelicStruct.h"
#include "Potion/PotionStruct.h"
#include "ChampionStruct.generated.h"

class AUnit;

// 파티원 1명의 덱 카드 목록 (PawnIndex별로 배열로 관리)
USTRUCT(BlueprintType)
struct FDeckSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Deck")
	TArray<FName> Cards;
};

/*Champion 정보*/
USTRUCT(BlueprintType)
struct FSavePartyInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "ChampionInstance", meta = (AllowPrivateAccess = "true"))
	int32 MaxGold = 9999;

	UPROPERTY(BlueprintReadOnly, Category = "ChampionInstance", meta = (AllowPrivateAccess = "true"))
	int32 Gold =0;
	UPROPERTY(BlueprintReadOnly, Category = "ChampionInstance", meta = (AllowPrivateAccess = "true"))
	TArray<FName> PartyMemberIDs;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "ChampionInstance", meta = (AllowPrivateAccess = "true"))
	TArray<AUnit*> Champions;

	UPROPERTY(BlueprintReadOnly, Category = "ChampionInstance", meta = (AllowPrivateAccess = "true"))
	TArray<int32> Deck;

	UPROPERTY(BlueprintReadOnly, Category = "ChampionInstance", meta = (AllowPrivateAccess = "true"))
	TArray<FRelic> Relics;

	UPROPERTY(BlueprintReadOnly, Category = "ChampionInstance", meta = (AllowPrivateAccess = "true"))
	TArray<FPotionData> Potions;

	UPROPERTY(BlueprintReadOnly, Category = "ChampionInstance")
	TArray<int32> ChampionCurrentHPs;

	UPROPERTY(BlueprintReadOnly, Category = "ChampionInstance")
	TArray<int32> ChampionMaxHPs;

	// 파티원별 덱 (인덱스 = PawnIndex). 보상 카드 포함 전체 덱 상태 저장.
	UPROPERTY(BlueprintReadOnly, Category = "ChampionInstance")
	TArray<FDeckSaveData> ChampionDecks;
public:

	FSavePartyInfo()
	{
	}

	void InitSavePartyInfo()
	{
		MaxGold = 9999;
		Gold = 0;
		PartyMemberIDs.Empty();
		Champions.Empty();
		Deck.Empty();
		Relics.Empty();
		Potions.Empty();
		ChampionCurrentHPs.Empty();
		ChampionMaxHPs.Empty();
		ChampionDecks.Empty();
	}
};
