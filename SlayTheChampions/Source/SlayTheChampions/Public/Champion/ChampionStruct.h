// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChampionStruct.generated.h"

class AUnit;

/*Champion Á¤º¸*/
USTRUCT(BlueprintType)
struct FSaveChampionsInfo
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadOnly, Category = "ChampionInstance", meta = (AllowPrivateAccess = "true"))
	int32 MaxGold;

	UPROPERTY(BlueprintReadOnly, Category = "ChampionInstance", meta = (AllowPrivateAccess = "true"))
	int32 Gold;

	UPROPERTY(BlueprintReadOnly, Category = "ChampionInstance", meta = (AllowPrivateAccess = "true"))
	TArray<AUnit*> Party;

	UPROPERTY(BlueprintReadOnly, Category = "ChampionInstance", meta = (AllowPrivateAccess = "true"))
	TArray<int32> Deck;

public:

	FSaveChampionsInfo()
	{
	}

	const int32 GetMaxGold() const { return MaxGold; }

	const int32 GetGold() const { return Gold; }

	const TArray<AUnit*> GetParty() const { return Party; }

	const TArray<int32> GetDeck() const { return Deck; }

	void SetMaxGold(int32 _MaxGold) { MaxGold = _MaxGold; }

	void SetGold(int32 _Gold) { Gold = _Gold; }

	void SetParty(TArray<AUnit*> _Party) { Party = _Party; }

	void SetDeck(TArray<int32> _Deck) { Deck = _Deck; }
};