#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Map/MapStruct.h"
#include "RunAreaClearWidget.generated.h"

class URewardSystem;

UCLASS(Blueprintable, BlueprintType)
class SLAYTHECHAMPIONS_API URunAreaClearWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	/*클리어된 Area 정보*/
	UPROPERTY(BlueprintReadOnly, Category = "AreaClear", meta = (AllowPrivateAccess = "true"))
	FAreaInfo ClearedAreaInfo;

	/*현재 클리어 보상으로 받을 골드*/
	UPROPERTY(BlueprintReadOnly, Category = "AreaClear", meta = (AllowPrivateAccess = "true"))
	int32 PendingGoldReward = 0;

	/*현재 보유 골드*/
	UPROPERTY(BlueprintReadOnly, Category = "AreaClear", meta = (AllowPrivateAccess = "true"))
	int32 CurrentGold = 0;

	/*보상 수령 여부*/
	UPROPERTY(BlueprintReadOnly, Category = "AreaClear", meta = (AllowPrivateAccess = "true"))
	bool bRewardClaimed = false;

	/*보상 시스템 참조*/
	UPROPERTY()
	URewardSystem* RewardSystem = nullptr;

public:
	/*Area 클리어 보상 위젯 초기화*/
	void SetupAreaClearWidget(URewardSystem* InRewardSystem, const FAreaInfo& InClearedAreaInfo, int32 InPendingGoldReward, int32 InCurrentGold);

	/*보상 수령 확인 처리*/
	UFUNCTION(BlueprintCallable, Category = "AreaClear")
	void ConfirmReward();

	/*맵으로 돌아가기 처리*/
	UFUNCTION(BlueprintCallable, Category = "AreaClear")
	void ReturnToMap();

	/*Area 클리어 보상 위젯이 열렸을 때 Blueprint에서 구현할 이벤트*/
	UFUNCTION(BlueprintImplementableEvent, Category = "AreaClear")
	void OnAreaClearOpened();

	/*보상이 수령되었을 때 Blueprint에서 구현할 이벤트*/
	UFUNCTION(BlueprintImplementableEvent, Category = "AreaClear")
	void OnRewardClaimed();
};
