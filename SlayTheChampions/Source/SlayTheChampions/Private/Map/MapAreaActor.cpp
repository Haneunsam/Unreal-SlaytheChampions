#include "Map/MapAreaActor.h"

#include "Components/PrimitiveComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameManagers/LevelManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Map/RunSystem.h"

AMapAreaActor::AMapAreaActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMapAreaActor::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		PlayerController->bEnableClickEvents = true;
		PlayerController->bShowMouseCursor = true;
	}
}

void AMapAreaActor::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);

	// 연출 BP가 먼저 반응할 수 있도록 클릭/선택 이벤트를 먼저 알린다.
	OnAreaClicked.Broadcast(this);
	OnAreaSelected.Broadcast(this, FloorIndex, RoomIndex);

	if (bAutoMoveToTargetLevelOnClick)
	{
		MoveToTargetLevel();
	}
}

void AMapAreaActor::SetAreaIndex(int32 InFloorIndex, int32 InRoomIndex)
{
	FloorIndex = InFloorIndex;
	RoomIndex = InRoomIndex;
}

void AMapAreaActor::ApplyDebugAreaInfo(const FAreaInfo& InAreaInfo)
{
	FLinearColor DebugColor = FLinearColor::Green;
	if (InAreaInfo.AreaVisit == EAreaVisitState::Cleared || InAreaInfo.AreaVisit == EAreaVisitState::Visited)
	{
		DebugColor = FLinearColor::Blue;
	}
	else if (InAreaInfo.bCanEnter)
	{
		DebugColor = FLinearColor::Red;
	}

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	GetComponents<UPrimitiveComponent>(PrimitiveComponents);
	for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
	{
		if (!PrimitiveComponent)
		{
			continue;
		}

		const int32 MaterialCount = PrimitiveComponent->GetNumMaterials();
		for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; MaterialIndex++)
		{
			if (UMaterialInstanceDynamic* DynamicMaterial = PrimitiveComponent->CreateAndSetMaterialInstanceDynamic(MaterialIndex))
			{
				DynamicMaterial->SetVectorParameterValue(DebugColorParameterName, DebugColor);
				DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), DebugColor);
				DynamicMaterial->SetVectorParameterValue(TEXT("TintColor"), DebugColor);
				DynamicMaterial->SetVectorParameterValue(TEXT("DebugColor"), DebugColor);
			}
		}
	}
}

void AMapAreaActor::SetTargetLevelName(FName InTargetLevelName)
{
	TargetLevelName = InTargetLevelName;
}

void AMapAreaActor::MoveToTargetLevel()
{
	// 기본 클릭 이동 흐름: 런 시스템에 방 입장을 먼저 반영한 뒤 서브레벨을 전환한다.
	if (PrepareEnterTargetRoom())
	{
		ContinueMoveToTargetLevel();
	}
}

bool AMapAreaActor::PrepareEnterTargetRoom()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (URunSystem* RunSystem = GameInstance->GetSubsystem<URunSystem>())
		{
			if (!RunSystem->CanEnterRoomByGridIndex(FloorIndex, RoomIndex))
			{
				return false;
			}

			if (!RunSystem->EnterRoomByGridIndex(FloorIndex, RoomIndex))
			{
				return false;
			}
		}
	}

	return true;
}

void AMapAreaActor::ContinueMoveToTargetLevel()
{
	// 포탈 클릭 연출을 쓰는 BP에서는 PrepareEnterTargetRoom 이후 원하는 타이밍에 이 함수를 호출하면 된다.
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (ULevelManager* LevelManager = GameInstance->GetSubsystem<ULevelManager>())
		{
			if (!TargetLevelName.IsNone())
			{
				LevelManager->MoveToConfiguredLevel(TargetLevelName);
			}
		}
	}
}
