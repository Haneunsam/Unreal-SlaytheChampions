// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class Area;
class SLAYTHECHAMPIONS_API MapCreator
{
private:
	/*배열에서의 맵*/
	TArray<int32> GridMap = { 0 };

	/*현재 생성된 맵 데이터 배열*/
	TArray<Area*> Map;

	/*맵 너비*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapCreator", meta = (AllowPrivateAccess = "true"))
	int32 MapWidth = 7;

	/*맵 높이*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapCreator", meta = (AllowPrivateAccess = "true"))
	int32 MapHeight = 15;
public:
	MapCreator();
	~MapCreator();

	/*맵 생성*/
	void CreateMap();

	/*맵 너비 설정*/
	void SetMapWidth(int32 _width) { MapWidth = _width; }

	/*맵 높이 설정*/
	void SetMapHeight(int32 _height) { MapHeight = _height; }
private:
	/*그리드 맵 초기화*/
	void InitGridMap() { GridMap.SetNum(MapWidth * MapHeight); }

	/*실제 맵 초기화*/
	void InitMap() { Map.Empty(); }

	/*그리드 맵 생성*/
	void GridMapCreate();
	
	/*층별 랜덤 방위치 반환*/
	int32 GetRandAreaPos(int32 _min, int32 _max);
};
