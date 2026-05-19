#include "Map/RunAreaClearWidget.h"

#include "Map/RewardSystem.h"

void URunAreaClearWidget::SetupAreaClearWidget(URewardSystem* InRewardSystem, const FAreaInfo& InClearedAreaInfo, int32 InPendingGoldReward, int32 InCurrentGold)
{
	RewardSystem = InRewardSystem;
	ClearedAreaInfo = InClearedAreaInfo;
	PendingGoldReward = InPendingGoldReward;
	CurrentGold = InCurrentGold;
	bRewardClaimed = false;
	OnAreaClearOpened();
}

void URunAreaClearWidget::ConfirmReward()
{
	if (!RewardSystem || bRewardClaimed)
	{
		return;
	}

	RewardSystem->ClaimAreaClearReward();
	CurrentGold = RewardSystem->GetCurrentGold();
	bRewardClaimed = true;
	OnRewardClaimed();
}

void URunAreaClearWidget::ReturnToMap()
{
	if (!RewardSystem)
	{
		return;
	}

	RewardSystem->ReturnToMapAfterReward();
}
