#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Card/CardDataTypes.h"
#include "Item/ItemActor.h"
#include "ShopSystem.generated.h"

class USceneComponent;
class UItemVisualDataAsset;
class ACardShopFrameActor;

UENUM(BlueprintType)
/* 상점에서 판매할 아이템 종류 */
enum class EShopSaleItemType : uint8
{
	Card = 0 UMETA(DisplayName = "Card"),
	Relic = 1 UMETA(DisplayName = "Relic"),
	Potion = 2 UMETA(DisplayName = "Potion"),
};

UCLASS()
class SLAYTHECHAMPIONS_API AShopSystem : public AActor
{
	GENERATED_BODY()

public:
	AShopSystem();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop|Component")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	bool bSpawnOnBeginPlay = true;

	/* 카드 액자 방식을 쓰지 않을 때 사용할 단일 카드 판매 액터 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Class")
	TSubclassOf<AActor> CardSaleActorClass;

	/* 여러 장의 카드를 한 액자 안에 보여주는 판매 액터 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Class")
	TSubclassOf<ACardShopFrameActor> CardShopFrameActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Class")
	TSubclassOf<AItemActor> RelicItemActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Class")
	TSubclassOf<AItemActor> PotionItemActorClass;

	/* 유물/포션 아이템 ID에 맞는 메쉬 데이터를 연결하는 데이터 에셋 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Visual")
	TObjectPtr<UItemVisualDataAsset> ItemVisualDataAsset;

	/* 카드 판매 위치. 테이블 BP에서 RegisterSpawnPoint로 등록한다. */
	UPROPERTY(BlueprintReadOnly, Category = "Shop|SpawnPoint")
	TArray<TObjectPtr<USceneComponent>> CardSpawnPoints;

	/* 유물 판매 위치. */
	UPROPERTY(BlueprintReadOnly, Category = "Shop|SpawnPoint")
	TArray<TObjectPtr<USceneComponent>> RelicSpawnPoints;

	/* 포션 판매 위치. */
	UPROPERTY(BlueprintReadOnly, Category = "Shop|SpawnPoint")
	TArray<TObjectPtr<USceneComponent>> PotionSpawnPoints;

	/* 현재 상점에 생성된 판매 액터 목록 */
	UPROPERTY(BlueprintReadOnly, Category = "Shop|Runtime")
	TArray<TObjectPtr<AActor>> SpawnedSaleActors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Card")
	EJobClass ShopCardClass = EJobClass::Any;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Card")
	ECardRarity ShopCardMinRarity = ECardRarity::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Card")
	int32 CardsPerFrame = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Price")
	int32 CardMinPrice = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Price")
	int32 CardMaxPrice = 120;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Price")
	int32 RelicMinPrice = 120;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Price")
	int32 RelicMaxPrice = 220;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Price")
	int32 PotionMinPrice = 40;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Price")
	int32 PotionMaxPrice = 90;

public:
	/* 테이블/진열대 BP가 자기 스폰 포인트를 상점 시스템에 등록할 때 사용한다. */
	UFUNCTION(BlueprintCallable, Category = "Shop|SpawnPoint")
	void RegisterSpawnPoint(EShopSaleItemType SaleItemType, USceneComponent* SpawnPoint);

	UFUNCTION(BlueprintCallable, Category = "Shop|SpawnPoint")
	void ClearRegisteredSpawnPoints();

	/* 등록된 스폰 포인트들에 카드/유물/포션 판매 아이템을 생성한다. */
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void SpawnShopItems();

	/* 이미 생성된 판매 액터를 제거한다. 상점 재입장/재스폰 전에 호출한다. */
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void ClearShopItems();

	UFUNCTION(BlueprintImplementableEvent, Category = "Shop|Card")
	void OnCardSaleItemSpawned(AActor* SpawnedActor, FName CardID);

	UFUNCTION(BlueprintImplementableEvent, Category = "Shop|Card")
	void OnCardShopFrameSpawned(ACardShopFrameActor* SpawnedFrame);

	UFUNCTION(BlueprintImplementableEvent, Category = "Shop")
	void OnShopItemsSpawned();

protected:
	FName GetRandomShopCardID() const;

	FName GetRandomShopRelicID() const;

	FName GetRandomShopPotionID() const;

	int32 RollPrice(int32 MinPrice, int32 MaxPrice) const;

	void SpawnCardSaleItem(USceneComponent* SpawnPoint);

	void SpawnRelicSaleItem(USceneComponent* SpawnPoint);

	void SpawnPotionSaleItem(USceneComponent* SpawnPoint);

	AActor* SpawnActorAtPoint(TSubclassOf<AActor> ActorClass, USceneComponent* SpawnPoint);
};
