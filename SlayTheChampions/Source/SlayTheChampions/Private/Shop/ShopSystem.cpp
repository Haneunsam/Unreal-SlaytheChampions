#include "Shop/ShopSystem.h"

#include "Card/CardSubsystem.h"
#include "Components/SceneComponent.h"
#include "Potion/PotionSubsystem.h"
#include "Relic/RelicSubsystem.h"
#include "Shop/CardShopFrameActor.h"

AShopSystem::AShopSystem()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
}

void AShopSystem::BeginPlay()
{
	Super::BeginPlay();

	if (bSpawnOnBeginPlay)
	{
		SpawnShopItems();
	}
}

void AShopSystem::RegisterSpawnPoint(EShopSaleItemType SaleItemType, USceneComponent* SpawnPoint)
{
	if (!SpawnPoint)
	{
		return;
	}

	switch (SaleItemType)
	{
	case EShopSaleItemType::Card:
		CardSpawnPoints.AddUnique(SpawnPoint);
		break;
	case EShopSaleItemType::Relic:
		RelicSpawnPoints.AddUnique(SpawnPoint);
		break;
	case EShopSaleItemType::Potion:
		PotionSpawnPoints.AddUnique(SpawnPoint);
		break;
	default:
		break;
	}
}

void AShopSystem::ClearRegisteredSpawnPoints()
{
	CardSpawnPoints.Reset();
	RelicSpawnPoints.Reset();
	PotionSpawnPoints.Reset();
}

void AShopSystem::SpawnShopItems()
{
	ClearShopItems();

	// BP 테이블들이 등록한 스폰 포인트 종류에 맞춰 각각의 판매 아이템을 생성한다.
	for (USceneComponent* SpawnPoint : CardSpawnPoints)
	{
		SpawnCardSaleItem(SpawnPoint);
	}

	for (USceneComponent* SpawnPoint : RelicSpawnPoints)
	{
		SpawnRelicSaleItem(SpawnPoint);
	}

	for (USceneComponent* SpawnPoint : PotionSpawnPoints)
	{
		SpawnPotionSaleItem(SpawnPoint);
	}

	OnShopItemsSpawned();
}

void AShopSystem::ClearShopItems()
{
	for (AActor* SpawnedActor : SpawnedSaleActors)
	{
		if (IsValid(SpawnedActor))
		{
			SpawnedActor->Destroy();
		}
	}

	SpawnedSaleActors.Reset();
}

FName AShopSystem::GetRandomShopCardID() const
{
	const UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return NAME_None;
	}

	const UCardSubsystem* CardSubsystem = GameInstance->GetSubsystem<UCardSubsystem>();
	if (!CardSubsystem)
	{
		return NAME_None;
	}

	const TArray<FName> CardPool = CardSubsystem->GetRewardPool(ShopCardClass, ShopCardMinRarity);
	if (CardPool.IsEmpty())
	{
		return NAME_None;
	}

	const int32 Index = FMath::RandRange(0, CardPool.Num() - 1);
	return CardPool[Index];
}

FName AShopSystem::GetRandomShopRelicID() const
{
	const UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return NAME_None;
	}

	URelicSubsystem* RelicSubsystem = GameInstance->GetSubsystem<URelicSubsystem>();
	return RelicSubsystem ? RelicSubsystem->GetRandomShopAvailableRelic() : NAME_None;
}

FName AShopSystem::GetRandomShopPotionID() const
{
	const UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return NAME_None;
	}

	const UPotionSubsystem* PotionSubsystem = GameInstance->GetSubsystem<UPotionSubsystem>();
	return PotionSubsystem ? PotionSubsystem->GetRandomAnyPotion() : NAME_None;
}

int32 AShopSystem::RollPrice(int32 MinPrice, int32 MaxPrice) const
{
	// 에디터에서 최소/최대 가격이 뒤집혀도 안전하게 랜덤 가격을 뽑는다.
	const int32 SafeMinPrice = FMath::Max(0, MinPrice);
	const int32 SafeMaxPrice = FMath::Max(SafeMinPrice, MaxPrice);
	return FMath::RandRange(SafeMinPrice, SafeMaxPrice);
}

void AShopSystem::SpawnCardSaleItem(USceneComponent* SpawnPoint)
{
	if (CardShopFrameActorClass)
	{
		// 카드 액자를 쓰는 경우 액자 하나에 여러 카드 ID와 가격을 넘긴다.
		ACardShopFrameActor* SpawnedFrame = Cast<ACardShopFrameActor>(SpawnActorAtPoint(CardShopFrameActorClass, SpawnPoint));
		if (!SpawnedFrame)
		{
			return;
		}

		TArray<FName> CardIDs;
		TArray<int32> Prices;
		const int32 SafeCardsPerFrame = FMath::Max(1, CardsPerFrame);
		for (int32 Index = 0; Index < SafeCardsPerFrame; ++Index)
		{
			const FName CardID = GetRandomShopCardID();
			if (CardID.IsNone())
			{
				continue;
			}

			CardIDs.Add(CardID);
			Prices.Add(RollPrice(CardMinPrice, CardMaxPrice));
		}

		SpawnedFrame->InitCards(CardIDs, Prices);
		SpawnedSaleActors.Add(SpawnedFrame);
		OnCardShopFrameSpawned(SpawnedFrame);
		return;
	}

	const FName CardID = GetRandomShopCardID();
	if (CardID.IsNone())
	{
		return;
	}

	AActor* SpawnedActor = SpawnActorAtPoint(CardSaleActorClass, SpawnPoint);
	if (!SpawnedActor)
	{
		return;
	}

	SpawnedSaleActors.Add(SpawnedActor);
	OnCardSaleItemSpawned(SpawnedActor, CardID);
}

void AShopSystem::SpawnRelicSaleItem(USceneComponent* SpawnPoint)
{
	const FName RelicID = GetRandomShopRelicID();
	if (RelicID.IsNone())
	{
		return;
	}

	AItemActor* SpawnedActor = Cast<AItemActor>(SpawnActorAtPoint(RelicItemActorClass, SpawnPoint));
	if (!SpawnedActor)
	{
		return;
	}

	// 유물 ID에 맞는 메쉬/가격을 아이템 액터에 주입한다.
	SpawnedActor->SetItemVisualDataAsset(ItemVisualDataAsset);
	SpawnedActor->InitItem(EItemActorType::Relic, RelicID);
	SpawnedActor->SetPrice(RollPrice(RelicMinPrice, RelicMaxPrice));
	SpawnedSaleActors.Add(SpawnedActor);
}

void AShopSystem::SpawnPotionSaleItem(USceneComponent* SpawnPoint)
{
	const FName PotionID = GetRandomShopPotionID();
	if (PotionID.IsNone())
	{
		return;
	}

	AItemActor* SpawnedActor = Cast<AItemActor>(SpawnActorAtPoint(PotionItemActorClass, SpawnPoint));
	if (!SpawnedActor)
	{
		return;
	}

	// 포션 ID에 맞는 메쉬/가격을 아이템 액터에 주입한다.
	SpawnedActor->SetItemVisualDataAsset(ItemVisualDataAsset);
	SpawnedActor->InitItem(EItemActorType::Potion, PotionID);
	SpawnedActor->SetPrice(RollPrice(PotionMinPrice, PotionMaxPrice));
	SpawnedSaleActors.Add(SpawnedActor);
}

AActor* AShopSystem::SpawnActorAtPoint(TSubclassOf<AActor> ActorClass, USceneComponent* SpawnPoint)
{
	if (!ActorClass || !SpawnPoint)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	return World->SpawnActor<AActor>(
		ActorClass,
		SpawnPoint->GetComponentLocation(),
		SpawnPoint->GetComponentRotation(),
		SpawnParams);
}
