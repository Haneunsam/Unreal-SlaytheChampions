#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Map/MapStruct.h"
#include "MapAreaActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapAreaClicked, AMapAreaActor*, AreaActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnMapAreaSelected, AMapAreaActor*, AreaActor, int32, FloorIndex, int32, RoomIndex);

UCLASS()
class SLAYTHECHAMPIONS_API AMapAreaActor : public AActor
{
	GENERATED_BODY()

public:
	AMapAreaActor();

protected:
	virtual void BeginPlay() override;
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;

public:
	/* 포탈이 클릭된 직후 호출된다. 레벨 이동 연출을 바인드할 때 사용한다. */
	UPROPERTY(BlueprintAssignable, Category = "Area|Event")
	FOnMapAreaClicked OnAreaClicked;

	/* 클릭된 포탈의 맵 좌표까지 함께 알려준다. */
	UPROPERTY(BlueprintAssignable, Category = "Area|Event")
	FOnMapAreaSelected OnAreaSelected;

	UFUNCTION(BlueprintCallable)
	void SetAreaIndex(int32 InFloorIndex, int32 InRoomIndex);

	UFUNCTION(BlueprintCallable)
	void ApplyDebugAreaInfo(const FAreaInfo& InAreaInfo);

	UFUNCTION(BlueprintCallable)
	void SetTargetLevelName(FName InTargetLevelName);

	UFUNCTION(BlueprintCallable)
	void MoveToTargetLevel();

	/* RunSystem에 방 입장만 먼저 반영한다. 연출 후 이동하고 싶을 때 분리해서 사용한다. */
	UFUNCTION(BlueprintCallable)
	bool PrepareEnterTargetRoom();

	/* 이미 준비된 목표 레벨로 실제 서브레벨 전환을 실행한다. */
	UFUNCTION(BlueprintCallable)
	void ContinueMoveToTargetLevel();

	int32 GetFloorIndex() const { return FloorIndex; }
	int32 GetRoomIndex() const { return RoomIndex; }

private:
	UPROPERTY(EditAnywhere, Category = "Area")
	FName TargetLevelName;

	UPROPERTY(EditAnywhere, Category = "Area")
	int32 FloorIndex = INDEX_NONE;

	UPROPERTY(EditAnywhere, Category = "Area")
	int32 RoomIndex = INDEX_NONE;

	UPROPERTY(EditAnywhere, Category = "Area")
	FName DebugColorParameterName = TEXT("Color");

	/* false로 두면 클릭 시 자동 이동하지 않고 OnAreaClicked를 받은 BP가 이동 타이밍을 결정한다. */
	UPROPERTY(EditAnywhere, Category = "Area")
	bool bAutoMoveToTargetLevelOnClick = true;
};
