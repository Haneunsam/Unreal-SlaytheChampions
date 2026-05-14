// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Map/MapEnum.h"

/*Area Á¤º¸*/
USTRUCT()
struct FAreaInfo
{
	FName Area_Name;
	EAreaState Area_State;
	EAreaType Area_Type;
	FVector Area_WorldPos;
	FVector2D Area_ArrPos;
};
