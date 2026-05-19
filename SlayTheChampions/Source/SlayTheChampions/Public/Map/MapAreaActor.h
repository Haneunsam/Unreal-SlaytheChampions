#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Map/MapStruct.h"
#include "MapAreaActor.generated.h"

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
	UFUNCTION(BlueprintCallable)
	void SetAreaIndex(int32 InFloorIndex, int32 InRoomIndex);

	UFUNCTION(BlueprintCallable)
	void ApplyDebugAreaInfo(const FAreaInfo& InAreaInfo);

	UFUNCTION(BlueprintCallable)
	void SetTargetLevelName(FName InTargetLevelName);

	UFUNCTION(BlueprintCallable)
	void MoveToTargetLevel();

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
};
