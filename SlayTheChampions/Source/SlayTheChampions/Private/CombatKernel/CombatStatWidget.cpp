#include "CombatKernel/CombatStatWidget.h"
#include "Unit/Unit.h"
#include "Unit/StatComponent.h"
#include "Unit/StatusEffectComponent.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UCombatStatWidget::InitFromUnit(AUnit* InUnit)
{
	if (!InUnit) return;

	// StatComponent 연결 → HP 표시
	UnitStatComp = InUnit->GetStat();
	if (UnitStatComp)
	{
		UnitStatComp->OnHPChanged.AddDynamic(this, &UCombatStatWidget::OnUnitHPChanged);
		OnUnitHPChanged(0, UnitStatComp->CurrentHP);
	}

	// StatusEffectComponent 연결 → Block 표시
	UStatusEffectComponent* SEC = InUnit->FindComponentByClass<UStatusEffectComponent>();
	if (SEC)
	{
		SEC->OnEffectValueChanged.AddDynamic(this, &UCombatStatWidget::OnBlockValueChanged);
	}

	// Block 초기값 (0 → 숨김)
	UpdateBlockVisibility(0);
}

void UCombatStatWidget::OnUnitHPChanged(int32 OldValue, int32 NewValue)
{
	if (Text_HP && UnitStatComp)
		Text_HP->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), NewValue, UnitStatComp->MaxHP)));
}

void UCombatStatWidget::OnBlockValueChanged(EEffectType Type, int32 OldValue, int32 NewValue)
{
	if (Type != EEffectType::Block) return;

	if (Text_Defence)
		Text_Defence->SetText(FText::AsNumber(NewValue));

	UpdateBlockVisibility(NewValue);
}

void UCombatStatWidget::UpdateBlockVisibility(int32 BlockValue)
{
	const ESlateVisibility BlockVisibility = BlockValue > 0
		? ESlateVisibility::HitTestInvisible
		: ESlateVisibility::Collapsed;

	if (Text_Defence) Text_Defence->SetVisibility(BlockVisibility);
	if (Image_Block)  Image_Block->SetVisibility(BlockVisibility);
}
