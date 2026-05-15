// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MapManager.generated.h"

class UMapCreator;
class UMapConfigData;
UCLASS(BlueprintType, Blueprintable)
class SLAYTHECHAMPIONS_API UMapManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
private:
	UPROPERTY()
	UMapCreator* MapCreator;

private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
public:
	/*맵 생성 호출*/
	UFUNCTION(BlueprintCallable)
	void MapCreate();
	//맵 생성 호출
	//맵 이동
	//몇층인지 몇번째 레벨인지
	//노드 입장 이벤트
	// 그리드 화면 Ui등
	// 스테이지 클리어 상태 확인	
};
