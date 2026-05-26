// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit/Enemy/GimmickComponent.h"
#include "Gimmick_CurseInserter.generated.h"

/**
 * 플레이어에게 덱순환을 방해하는 저주카드를 삽입
 */
 // 덱 시스템이 구독: TargetTeam에 해당하는 파티원 덱에 저주 삽입
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurseInsertRequested, int32, CurseID);

UCLASS(ClassGroup = (Unit), meta = (BlueprintSpawnableComponent))
class SLAYTHECHAMPIONS_API UGimmick_CurseInserter : public UGimmickComponent
{
	GENERATED_BODY()
	
public:
	
	//몇턴마다 삽입할지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Curse")
	int32 InsertInterval = 2;

	//삽입할 저주 카드 ID(데이터 테이블 키)
	int32 CurseCardID = 0;

	// 삽입 연출 텍스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Curse")
	FText InsertAnnounce;

	virtual bool WillTriggerNextTurn() const override;

	// 지금까지 삽입된 저주 수 (UI 표시용)
	UPROPERTY(BlueprintReadOnly, Category = "Gimmick|Curse")
	int32 TotalCursesInserted = 0;


	// 덱 시스템이 구독
	UPROPERTY(BlueprintAssignable, Category = "Gimmick|Curse")
	FOnCurseInsertRequested OnCurseInsertRequested;



protected:
	virtual void OnGimmickTurnStart() override;

};
