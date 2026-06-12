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
	UPROPERTY(BlueprintAssignable, Category = "Area|Event")
	FOnMapAreaClicked OnAreaClicked;

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

	UFUNCTION(BlueprintCallable)
	bool PrepareEnterTargetRoom();

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

	UPROPERTY(EditAnywhere, Category = "Area")
	bool bAutoMoveToTargetLevelOnClick = true;
};
