// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAreaState : uint8
{
	Start = 0    UMETA(DisplayName = "시작"),
	Runing       UMETA(DisplayName = "진행중"),
	End			 UMETA(DisplayName = "종료")
};

UENUM(BlueprintType)
enum class EAreaType : uint8
{
	Nomal = 0	UMETA(DisplayName = "기본"),
	Elite		UMETA(DisplayName = "엘리트"),
	Boss		UMETA(DisplayName = "보스"),
	Event		UMETA(DisplayName = "이벤트"),
	Rest		UMETA(DisplayName = "휴식"),
	Shop		UMETA(DisplayName = "상점")
};