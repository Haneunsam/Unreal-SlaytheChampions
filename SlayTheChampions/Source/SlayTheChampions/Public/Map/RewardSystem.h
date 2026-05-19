#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Map/MapStruct.h"
#include "RewardSystem.generated.h"

class URunAreaClearWidget;
class URunSystem;
class UAreaClearUiData;

UCLASS(BlueprintType)
class SLAYTHECHAMPIONS_API URewardSystem : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY()
	URunSystem* RunSystem = nullptr;

	/*현재 활성화된 Area 클리어 위젯*/
	UPROPERTY()
	URunAreaClearWidget* ActiveAreaClearWidget = nullptr;

	/*Area 클리어 보상 UI 데이터 에셋*/
	UPROPERTY()
	UAreaClearUiData* AreaClearUiData = nullptr;

	/*클리어된 Area 정보*/
	UPROPERTY()
	FAreaInfo ClearedAreaInfo;

	/*Area 타입별 클리어 보상 골드*/
	UPROPERTY()
	int32 NormalAreaClearGold = 10;

	UPROPERTY()
	int32 EliteAreaClearGold = 25;

	UPROPERTY()
	int32 BossAreaClearGold = 50;

	/*현재 보유 골드*/
	UPROPERTY()
	int32 CurrentGold = 0;

	/*현재 클리어 보상으로 받을 골드*/
	UPROPERTY()
	int32 PendingGoldReward = 0;

public:
	void Initialize(URunSystem* InRunSystem);
	/*Area 클리어 보상 처리*/
	UFUNCTION(BlueprintCallable)
	void OpenAreaClearReward(const FAreaInfo& InClearedAreaInfo);

	/*Area 클리어 보상 수령 처리*/
	UFUNCTION(BlueprintCallable)
	void ClaimAreaClearReward();

	/*Area 클리어 보상 수령 후 맵으로 복귀 처리*/
	UFUNCTION(BlueprintCallable)
	void ReturnToMapAfterReward();

	/*Area 타입별 클리어 보상 골드 설정*/
	UFUNCTION(BlueprintCallable)
	void SetAreaClearGoldRewardValues(int32 InNormalAreaClearGold, int32 InEliteAreaClearGold, int32 InBossAreaClearGold);

	/*Area 클리어 보상 UI 데이터 설정*/
	UFUNCTION(BlueprintCallable)
	void SetAreaClearUiData(UAreaClearUiData* InAreaClearUiData);

	/*현재 보유 골드 반환*/
	UFUNCTION(BlueprintPure)
	int32 GetCurrentGold() const { return CurrentGold; }

	/*현재 클리어 보상으로 받을 골드 반환*/
	UFUNCTION(BlueprintPure)
	int32 GetPendingGoldReward() const { return PendingGoldReward; }

private:
	/*Area 클리어 보상 계산*/
	bool LoadDefaultAreaClearUiData();
	/*Area 클리어 보상 계산*/
	int32 GetAreaClearGoldReward(const FAreaInfo& RoomInfo) const;
	/*Area 클리어 보상 UI 열기*/
	void OpenAreaClearWidget();
};
