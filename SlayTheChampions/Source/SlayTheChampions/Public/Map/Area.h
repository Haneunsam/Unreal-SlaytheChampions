#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Map/MapStruct.h"
#include "Area.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SLAYTHECHAMPIONS_API UArea : public USceneComponent
{
	GENERATED_BODY()

public:
	UArea();

protected:
	virtual void BeginPlay() override;

private:
	/*Area 정보 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area", meta = (AllowPrivateAccess = "true"))
	FAreaInfo AreaInfo;
	/*이동 가능한 Area 정보*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area", meta = (AllowPrivateAccess = "true"))
	TArray<UArea*> NextAreas;
public:
	/*Area 초기화*/
	void InitArea(const FAreaInfo& InInfo);
	/*Area 이름 반환*/
	FName GetAreaName() const { return AreaInfo.AreaName; }
	/*Area 월드 위치 반환*/
	FVector GetWorldPos() const { return AreaInfo.AreaWorldPos; }
	/*Area 배열 위치 반환*/
	FVector2D GetArrPos() const { return AreaInfo.AreaArrPos; }
	/*Area 상태 반환*/
	EAreaState GetState() const { return AreaInfo.AreaState; }
	/*Area 타입 반환*/
	EAreaType GetType() const { return AreaInfo.AreaType; }

	/*Area 상태 설정*/
	void SetState(EAreaState _state) { AreaInfo.AreaState = _state; }
	/*이동가능한 Area 추가*/
	void SetNextAreas(TArray<UArea*> _next) { NextAreas = _next; }
};
