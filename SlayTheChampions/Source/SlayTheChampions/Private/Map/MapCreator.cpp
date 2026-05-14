// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/MapCreator.h"

MapCreator::MapCreator()
{
}

MapCreator::~MapCreator()
{
}

void MapCreator::CreateMap()
{
	//맵 생성 작업전 정보 담을 배열들 초기화
	if (!Map.IsEmpty()) InitMap();
	if (!GridMap.IsEmpty()) InitGridMap();

	//1 그리드 맵 생성
	GridMapCreate();
	//2 실제 맵 생성 및 초기화
	//3 Map에 데이터 입력
}

void MapCreator::GridMapCreate()
{
	//조건 1 : 1번방은 ArtifactEvent
	//조건 2 : 9번방은 Reword
	//조건 3 : 15번방은 Rest
	//조건 4 : 16번방은 Boss
	//조건 5 : 모든방은 최소 1개이상의 연결된 노드가 필요함
	//조건 6 : 한 층에 최소 1개이상의 area가 있어야함
	//조건 7 : 연결된 방은 가장 가까운 거리 x-1 x x+1 y+1 방을 기준으로 연결하되 그 3곳에 방이 없으면 더 확장해서 연결


}

int32 MapCreator::GetRandAreaPos(int32 _min, int32 _max)
{

	return FMath::RandRange(_min,_max);
}
