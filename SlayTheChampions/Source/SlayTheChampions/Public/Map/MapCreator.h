// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MapCreator.generated.h"

class UArea;
class UMapConfigData;
enum class EAreaType : uint8;
UCLASS()
class SLAYTHECHAMPIONS_API UMapCreator : public UObject
{
	GENERATED_BODY()
private:
	/*배열에서의 맵*/
	UPROPERTY()
	TArray<bool> GridMap = { 0 };

	/*현재 생성된 맵 데이터 배열*/
	UPROPERTY()
	TArray<UArea*> Map;

	/*맵 생성 관련 데이터에셋*/
	UPROPERTY()
	UMapConfigData* MapConfig;
	
	int32 CurrentWidth;
	int32 CurrentHeight;
public:
	UMapCreator();
	~UMapCreator();

	/*맵 생성*/
	void CreateMap();

	/*맵 너비 설정*/
	void SetMapWidth(int32 _width) { CurrentWidth = _width; }

	/*맵 높이 설정*/
	void SetMapHeight(int32 _height) { CurrentHeight = _height; }
private:
	/*그리드 맵 초기화*/
	void InitGridMap() { GridMap.Init(false, CurrentWidth * CurrentHeight); }

	/*실제 맵 초기화*/
	void InitMap() { Map.Init(nullptr, CurrentWidth * CurrentHeight); }

	/*그리드 맵 생성*/
	void GridMapCreate(int32 MapWidth, int32 MapHeight, float AreaSpawnProbability);
	
	/*맵 데이터 생성*/
	void SetMapData(int32 MapWidth, int32 MapHeight);

	/*영역 생성*/
	UArea* AreaCreate(int32 height, int32 width);

	/*방 랜덤 타입 반환*/
	EAreaType GetRandAreaType();

	/*인접한 방 연결*/
	void ConnectAreas(int32 MapWidth, int32 MapHeight);

	/*연결가능한 방인지 확인*/
	bool ConnectIfValid(int32 CurrentIdx, int32 NextH, int32 NextW, int32 MapWidth);

	/*실제 월드 맵 생성*/
	void WorldMapCreate();
};
