// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MapManager.generated.h"

class UArea;
class UMapCreator;

UCLASS(BlueprintType, Blueprintable)
class SLAYTHECHAMPIONS_API UMapManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UMapCreator* MapCreator = nullptr;

private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	/* RunSystem 시작 호출 */
	UFUNCTION(BlueprintCallable)
	void StartRunSystem();

	/* 맵 생성 호출 */
	UFUNCTION(BlueprintCallable)
	void MapCreate();

	/* 디버그용 */
	void RestoreMapWorld();

	/* 디버그용 */
	bool HasMapData() const;

	/* 디버그용 */
	void RefreshDebugMapState();

	/* RunSystem 내부 조회용 */
	UArea* GetAreaAt(int32 height, int32 width) const;

	/* RunSystem 내부 조회용 */
	int32 GetMapWidth() const;

	/* RunSystem 내부 조회용 */
	int32 GetMapHeight() const;
};
