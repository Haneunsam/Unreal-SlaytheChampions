#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Map/MapStruct.h"
#include "PortalSpawner.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SLAYTHECHAMPIONS_API APortalSpawner : public AActor
{
	GENERATED_BODY()

public:
	APortalSpawner();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/* PortalCount 값으로 수동 포탈을 생성한다. */
	UFUNCTION(BlueprintCallable, Category = "PortalSpawner")
	void SpawnPortals();

	/* RunSystem의 현재 진입 가능 방 목록을 읽어서 포탈을 생성한다. */
	UFUNCTION(BlueprintCallable, Category = "PortalSpawner")
	void SpawnEnterableRoomPortals();

	/* 전달받은 방 정보 수만큼 포탈을 만들고 각 포탈에 맵 좌표와 목표 레벨을 설정한다. */
	UFUNCTION(BlueprintCallable, Category = "PortalSpawner")
	void SpawnPortalsForRooms(const TArray<FAreaInfo>& RoomInfos);

	UFUNCTION(BlueprintCallable, Category = "PortalSpawner")
	void ClearSpawnedPortals();

	UFUNCTION(BlueprintPure, Category = "PortalSpawner")
	const TArray<AActor*>& GetSpawnedPortals() const { return SpawnedPortals; }

protected:
	UFUNCTION()
	void HandleEnterableRoomsUpdated(const TArray<FAreaInfo>& RoomInfos);

	void RetrySpawnEnterableRoomPortals();

	AActor* SpawnPortalAtIndex(int32 Index, int32 Count);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PortalSpawner")
	TSubclassOf<AActor> PortalActorClass;

	/* bUseEnterableRooms가 false일 때 사용할 수동 포탈 개수 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PortalSpawner", meta = (ClampMin = "1"))
	int32 PortalCount = 5;

	/* 스포너 위치를 중심으로 포탈을 배치할 반지름 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PortalSpawner", meta = (ClampMin = "0.0"))
	float Radius = 500.f;

	/* PortalSpacing이 0일 때 사용할 전체 호 각도 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PortalSpawner", meta = (ClampMin = "0.0", ClampMax = "360.0"))
	float ArcAngleDegrees = 120.f;

	/* 0보다 크면 포탈 사이의 월드 거리 간격을 기준으로 호 각도를 계산한다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PortalSpawner", meta = (ClampMin = "0.0"))
	float PortalSpacing = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PortalSpawner")
	float HeightOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PortalSpawner")
	bool bSpawnOnBeginPlay = true;

	/* true면 런 맵의 진입 가능 방 목록으로 포탈을 만든다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PortalSpawner")
	bool bUseEnterableRooms = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PortalSpawner", meta = (ClampMin = "0"))
	int32 EmptyRoomRetryCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PortalSpawner", meta = (ClampMin = "0.0"))
	float EmptyRoomRetryDelay = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PortalSpawner")
	bool bClearBeforeSpawn = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PortalSpawner")
	bool bUseSpawnerForwardDirection = true;

	/* true면 생성된 포탈들이 스포너 중심점을 바라보게 회전한다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PortalSpawner")
	bool bPortalLookAtSpawner = true;

	UPROPERTY(BlueprintReadOnly, Category = "PortalSpawner")
	TArray<AActor*> SpawnedPortals;

	int32 CurrentEmptyRoomRetryCount = 0;
};
