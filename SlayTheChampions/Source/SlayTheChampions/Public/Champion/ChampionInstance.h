// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Champion/ChampionStruct.h"
#include "ChampionInstance.generated.h"

/**
 *
 */
class AUnit;
UCLASS()
class SLAYTHECHAMPIONS_API UChampionInstance : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadOnly, Category = "ChampionInstance", meta = (AllowPrivateAccess = "true"))
	FSaveChampionsInfo ChampionsInfo;

	//유물 추가 필요
public:
	//저장할때마다 호출
	void UpdateChampionsInfo(FSaveChampionsInfo _info) { ChampionsInfo = _info; }
	
	const FSaveChampionsInfo GetChampionsInfo()const { return ChampionsInfo; }
	void SetChampionsInf(FSaveChampionsInfo _info) { ChampionsInfo = _info; }
};
