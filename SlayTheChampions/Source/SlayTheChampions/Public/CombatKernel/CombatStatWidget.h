#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Unit/StatusEffectComponent.h"
#include "CombatStatWidget.generated.h"

class UTextBlock;
class UImage;
class AUnit;
class UStatComponent;
class UStatusEffectComponent;

UCLASS()
class SLAYTHECHAMPIONS_API UCombatStatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// AUnit 하나로 HP + Block 모두 연결
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void InitFromUnit(AUnit* InUnit);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_HP;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Defence;

	// 방패 아이콘 - Block=0이면 숨김 (BP 디자이너에서 배치)
	UPROPERTY(meta = (BindWidgetOptional))
	UImage* Image_Block;

private:
	UPROPERTY()
	UStatComponent* UnitStatComp;

	UFUNCTION()
	void OnUnitHPChanged(int32 OldValue, int32 NewValue);

	UFUNCTION()
	void OnBlockValueChanged(EEffectType Type, int32 OldValue, int32 NewValue);

	void UpdateBlockVisibility(int32 BlockValue);
};
