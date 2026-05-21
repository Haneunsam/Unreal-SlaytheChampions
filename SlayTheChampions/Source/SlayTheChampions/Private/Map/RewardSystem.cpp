#include "Map/RewardSystem.h"

#include "Map/AreaClearUiData.h"
#include "Map/RunAreaClearWidget.h"
#include "Map/RunSystem.h"

void URewardSystem::Initialize(URunSystem* InRunSystem)
{
	RunSystem = InRunSystem;
	ActiveAreaClearWidget = nullptr;
	CurrentGold = 0;
	PendingGoldReward = 0;
	LoadDefaultAreaClearUiData();
}

void URewardSystem::OpenAreaClearReward(const FAreaInfo& InClearedAreaInfo)
{
	ClearedAreaInfo = InClearedAreaInfo;
	PendingGoldReward = GetAreaClearGoldReward(ClearedAreaInfo);
	OpenAreaClearWidget();
}

void URewardSystem::ClaimAreaClearReward()
{
	if (PendingGoldReward <= 0)
	{
		return;
	}

	CurrentGold += PendingGoldReward;
	PendingGoldReward = 0;
}

void URewardSystem::ReturnToMapAfterReward()
{
	if (ActiveAreaClearWidget)
	{
		ActiveAreaClearWidget->RemoveFromParent();
		ActiveAreaClearWidget = nullptr;
	}

	if (!RunSystem)
	{
		return;
	}

	RunSystem->ReturnToMapAfterAreaClear();
}

void URewardSystem::SetAreaClearGoldRewardValues(int32 InNormalAreaClearGold, int32 InEliteAreaClearGold, int32 InBossAreaClearGold)
{
	NormalAreaClearGold = InNormalAreaClearGold;
	EliteAreaClearGold = InEliteAreaClearGold;
	BossAreaClearGold = InBossAreaClearGold;
}

void URewardSystem::SetAreaClearUiData(UAreaClearUiData* InAreaClearUiData)
{
	AreaClearUiData = InAreaClearUiData;
}

bool URewardSystem::LoadDefaultAreaClearUiData()
{
	if (!AreaClearUiData)
	{
		AreaClearUiData = LoadObject<UAreaClearUiData>(nullptr, TEXT("/Game/04_Data/AreaClearUiData.AreaClearUiData"));
	}

	return AreaClearUiData != nullptr;
}

int32 URewardSystem::GetAreaClearGoldReward(const FAreaInfo& RoomInfo) const
{
	switch (RoomInfo.AreaType)
	{
	case EAreaType::Normal:
		return NormalAreaClearGold;
	case EAreaType::Elite:
		return EliteAreaClearGold;
	case EAreaType::Boss:
		return BossAreaClearGold;
	default:
		return 0;
	}
}

void URewardSystem::OpenAreaClearWidget()
{
	if (!RunSystem || ActiveAreaClearWidget)
	{
		return;
	}

	if (!LoadDefaultAreaClearUiData())
	{
		return;
	}

	const TSubclassOf<URunAreaClearWidget> AreaClearWidgetClass = AreaClearUiData->GetAreaClearWidgetClass(ClearedAreaInfo.AreaType);
	if (!AreaClearWidgetClass)
	{
		return;
	}

	if (UGameInstance* GameInstance = RunSystem->GetGameInstance())
	{
		ActiveAreaClearWidget = CreateWidget<URunAreaClearWidget>(GameInstance, AreaClearWidgetClass);
		if (ActiveAreaClearWidget)
		{
			ActiveAreaClearWidget->AddToViewport();
			ActiveAreaClearWidget->SetupAreaClearWidget(this, ClearedAreaInfo, PendingGoldReward, CurrentGold);
		}
	}
}
