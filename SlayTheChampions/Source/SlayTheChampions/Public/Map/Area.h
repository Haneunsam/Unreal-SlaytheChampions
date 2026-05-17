#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Map/MapStruct.h"
#include "Area.generated.h"

UCLASS(BlueprintType)
class SLAYTHECHAMPIONS_API UArea : public UObject
{
	GENERATED_BODY()

public:
	UArea();

protected:

private:
	/* Area 정보 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area", meta = (AllowPrivateAccess = "true"))
	FAreaInfo AreaInfo;

	/* 이동 가능한 다음 Area 정보 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area", meta = (AllowPrivateAccess = "true"))
	TArray<UArea*> NextAreas;

public:
	/* Area 초기화 */
	void InitArea(const FAreaInfo& InInfo);

	/* Area 배열 위치 반환 */
	FVector2D GetArrPos() const { return AreaInfo.AreaPos; }

	/* Area 정보 반환 */
	const FAreaInfo& GetAreaInfo() const { return AreaInfo; }

	/* Area 상태 반환 */
	EAreaState GetState() const { return AreaInfo.AreaState; }

	/* Area 타입 반환 */
	EAreaType GetType() const { return AreaInfo.AreaType; }

	/* 이동 가능한 다음 Area 정보 반환 */
	const TArray<UArea*>& GetNextAreas() const { return NextAreas; }

	/* Area 상태 설정 */
	void SetState(EAreaState _state) { AreaInfo.AreaState = _state; }

	/* Area 방문 상태 설정 */
	void SetVisitState(EAreaVisitState _visitState) { AreaInfo.AreaVisit = _visitState; }

	void SetCanEnter(bool bInCanEnter) { AreaInfo.bCanEnter = bInCanEnter; }

	void SetCurrentArea(bool bInIsCurrentArea) { AreaInfo.bIsCurrentArea = bInIsCurrentArea; }

	/* 이동 가능한 다음 Area 추가 */
	void SetNextAreas(UArea* _next) { NextAreas.Add(_next); }

	/* 디버그용 */
	void DebugShowInfo();
};
