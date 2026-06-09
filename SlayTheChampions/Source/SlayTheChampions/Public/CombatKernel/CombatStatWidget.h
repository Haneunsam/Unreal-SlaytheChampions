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
class UNiagaraSystemWidget;
class UBuffPanelWidget;

/**
 * UCombatStatWidget
 * 유닛 한 명의 HP·Shield 수치를 실시간으로 표시하는 위젯.
 * InitFromUnit()으로 AUnit에 연결하면 델리게이트를 통해 자동 갱신된다.
 */
UCLASS()
class SLAYTHECHAMPIONS_API UCombatStatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// AUnit 하나로 HP + Shield 모두 연결. BeginPlay 이후에 호출해야 한다
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void InitFromUnit(AUnit* InUnit);

protected:
	// HP 텍스트 (BindWidget — BP 디자이너에서 같은 이름으로 배치 필수)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_HP;

	// 방어도(Shield) 수치 텍스트
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Defence;

	// 유닛 이름 텍스트 — WBP에서 'Text_Name' 이름으로 배치
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* Text_Name;

	// 방패 아이콘 — Shield=0이면 숨김 (BindWidgetOptional: 없어도 크래시 없음)
	UPROPERTY(meta = (BindWidgetOptional))
	UImage* Image_Block;

	// 방어도 나이아가라 위젯 — Shield=0이면 숨김, WBP에서 'NiagaraShield' 이름으로 배치
	UPROPERTY(meta = (BindWidgetOptional))
	UNiagaraSystemWidget* NiagaraShield;

	// 버프/디버프 패널 — WBP에서 'BuffPanel' 이름으로 배치
	UPROPERTY(meta = (BindWidgetOptional))
	UBuffPanelWidget* BuffPanel;

private:
	// 연결된 유닛의 StatComponent 캐시
	UPROPERTY()
	UStatComponent* UnitStatComp;

	// StatComponent::OnHPChanged 수신 → Text_HP 갱신
	UFUNCTION()
	void OnUnitHPChanged(int32 OldValue, int32 NewValue);

	// StatusEffectComponent::OnEffectValueChanged 수신 → Shield UI 갱신
	UFUNCTION()
	void OnShieldValueChanged(EEffectType Type, int32 OldValue, int32 NewValue);

	// Shield 수치에 따라 Text_Defence·Image_Block 표시 여부를 전환
	void UpdateShieldVisibility(int32 ShieldValue);
};
