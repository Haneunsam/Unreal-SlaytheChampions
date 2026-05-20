#include "CombatKernel/CombatStatWidget.h"
#include "Unit/StatComponent.h"
#include "Components/TextBlock.h"

void UCombatStatWidget::InitWidgetFromUnit(UStatComponent* InStat)
{
	if (!InStat) return;
	UnitStatComp = InStat;

	UnitStatComp->OnHPChanged.AddDynamic(this, &UCombatStatWidget::OnUnitHPChanged);

	// 초기값 즉시 표시
	OnUnitHPChanged(0, UnitStatComp->CurrentHP);
}

void UCombatStatWidget::OnUnitHPChanged(int32 OldValue, int32 NewValue)
{
	// [임시] Defence는 UStatComponent에 없으므로 숨김 처리
	if (Text_Defence)
		Text_Defence->SetVisibility(ESlateVisibility::Collapsed);

	if (Text_HP && UnitStatComp)
		Text_HP->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), NewValue, UnitStatComp->MaxHP)));
}
