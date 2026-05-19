#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Map/MapEnum.h"
#include "AreaClearUiData.generated.h"

class URunAreaClearWidget;

UCLASS(BlueprintType)
class SLAYTHECHAMPIONS_API UAreaClearUiData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AreaClearUi")
	TSubclassOf<URunAreaClearWidget> BattleAreaClearWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AreaClearUi")
	TSubclassOf<URunAreaClearWidget> EventAreaClearWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AreaClearUi")
	TSubclassOf<URunAreaClearWidget> RestAreaClearWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AreaClearUi")
	TSubclassOf<URunAreaClearWidget> ShopAreaClearWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AreaClearUi")
	TSubclassOf<URunAreaClearWidget> RewardAreaClearWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AreaClearUi")
	TSubclassOf<URunAreaClearWidget> ArtifactEventAreaClearWidget;

	/* Area 타입에 따른 클리어 위젯 클래스 반환 */
	TSubclassOf<URunAreaClearWidget> GetAreaClearWidgetClass(EAreaType AreaType) const;
};
