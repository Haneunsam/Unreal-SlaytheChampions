// Fill out your copyright notice in the Description page of Project Settings.


#include "Save/STCGameInstance.h"
#include "Save/GameSaveSystem.h"
#include "Champion/ChampionInstance.h"
#include "Map/RunSystem.h"
#include "Kismet/GameplayStatics.h"

void USTCGameInstance::SaveGameData()
{
	UGameSaveSystem* SaveGameInstance = Cast<UGameSaveSystem>(UGameplayStatics::CreateSaveGameObject(UGameSaveSystem::StaticClass()));
	if (!SaveGameInstance) return;

	UChampionInstance* PlayerSub = GetSubsystem<UChampionInstance>();
	URunSystem* MapSub = GetSubsystem<URunSystem>();

	if (PlayerSub)
	{
		SaveGameInstance->SavedChampionsInfo = PlayerSub->GetChampionsInfo();
	}

	if (MapSub)
	{
		SaveGameInstance->SavedMapInfo = MapSub->GetMapInfo();
	}

	bool bSuccess = UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, 0);

}

void USTCGameInstance::LoadGameData()
{
	if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		return;
	}

	UGameSaveSystem* LoadedGame = Cast<UGameSaveSystem>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	if (!LoadedGame) return;

	UChampionInstance* PlayerSub = GetSubsystem<UChampionInstance>();
	URunSystem* MapSub = GetSubsystem<URunSystem>();

	// 4. 세이브 가방에서 꺼내서 서브시스템들에게 분배하기
	if (PlayerSub)
	{
		PlayerSub->SetChampionsInf(LoadedGame->SavedChampionsInfo);
	}

	if (MapSub)
	{
		MapSub->SetMapInfo(LoadedGame->SavedMapInfo);
	}

}
