// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Map/MapStruct.h"
#include "Map/MapEnum.h"
#include "Area.generated.h"

class SLAYTHECHAMPIONS_API UArea : public USceneComponent
{
	GENERATED_BODY()

public:

private:
	FAreaInfo AreaInfo;

public:	
	UArea() {};

	virtual void BeginPlay() override;

	/*Area 초기화*/
	void InitArea(FAreaInfo _info);

	/*Area 이름 반환*/
	FName const GetName() const { return AreaInfo.Area_Name; }
	/*Area 월드 포지션 반환*/
	FVector const GetWorldPos() const { return AreaInfo.Area_WorldPos; }
	/*Area 배열 포지션 반환*/
	FVector2D const GetArrPos() const { return AreaInfo.Area_ArrPos; }
	/*Area 진행 상태 반환*/
	EAreaState const GetState() const { return AreaInfo.Area_State; }
	/*Area 타입 반환*/
	EAreaType const GetType() const { return AreaInfo.Area_Type; }
};
