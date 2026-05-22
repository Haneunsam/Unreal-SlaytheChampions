#include "CombatKernel/CombatStatWidget.h"
#include "Unit/Unit.h"
#include "Unit/StatComponent.h"
#include "Unit/StatusEffectComponent.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

// 유닛의 StatComponent와 StatusEffectComponent를 연결해 HP·Shield 변경 이벤트를 구독
void UCombatStatWidget::InitFromUnit(AUnit* InUnit)
{
	if (!InUnit) return;

	// StatComponent 연결 → HP 표시
	UnitStatComp = InUnit->GetStat();
	if (UnitStatComp)
	{
		UnitStatComp->OnHPChanged.AddDynamic(this, &UCombatStatWidget::OnUnitHPChanged);
		// 초기 HP 값을 즉시 반영
		OnUnitHPChanged(0, UnitStatComp->CurrentHP);
	}

	// StatusEffectComponent 연결 → Shield 표시
	UStatusEffectComponent* SEC = InUnit->FindComponentByClass<UStatusEffectComponent>();
	if (SEC)
	{
		SEC->OnEffectValueChanged.AddDynamic(this, &UCombatStatWidget::OnShieldValueChanged);
	}

	// Shield 초기값 (0 → 숨김)
	UpdateShieldVisibility(0);
}

// HP가 변경될 때 호출. "현재HP / 최대HP" 형식으로 텍스트 갱신
void UCombatStatWidget::OnUnitHPChanged(int32 OldValue, int32 NewValue)
{
	if (Text_HP && UnitStatComp)
		Text_HP->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), NewValue, UnitStatComp->MaxHP)));
}

// 이펙트 값이 변경될 때 호출. Shield 타입일 때만 방어도 UI를 갱신
void UCombatStatWidget::OnShieldValueChanged(EEffectType Type, int32 OldValue, int32 NewValue)
{
	if (Type != EEffectType::Shield) return;

	if (Text_Defence)
		Text_Defence->SetText(FText::AsNumber(NewValue));

	UpdateShieldVisibility(NewValue);
}

// Shield 수치에 따라 방어도 텍스트와 아이콘의 표시 여부를 전환
void UCombatStatWidget::UpdateShieldVisibility(int32 ShieldValue)
{
	// Shield가 0이면 UI 요소를 숨기고, 1 이상이면 표시
	const ESlateVisibility ShieldVisibility = ShieldValue > 0
		? ESlateVisibility::HitTestInvisible
		: ESlateVisibility::Collapsed;

	if (Text_Defence) Text_Defence->SetVisibility(ShieldVisibility);
	if (Image_Block)  Image_Block->SetVisibility(ShieldVisibility);
}
