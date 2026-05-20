// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "STCGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SLAYTHECHAMPIONS_API USTCGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
private:
	/*세이브 파일 이름*/
	const FString SaveSlotName = TEXT("SaveData_Slot0");

public:
	/*게임 저장*/
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SaveGameData();

	/*게임 불러오기*/
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void LoadGameData();


};
