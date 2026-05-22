// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit/Enemy/GimmickComponent.h"
#include "Gimmick_CurseInserter.generated.h"

/**
 * 플레이어의 덱에 저주 카드를 삽입하는 기믹 클래스
 */
 // 이 시스템의 방식: TargetTeam에 해당하는 파티의 덱에 저주 카드 추가
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurseInsertRequested, int32, CurseID);

UCLASS(ClassGroup = (Unit), meta = (BlueprintSpawnableComponent))
class SLAYTHECHAMPIONS_API UGimmick_CurseInserter : public UGimmickComponent
{
	GENERATED_BODY()

public:

};
