#include "CombatKernel/EffectManager.h"
#include "Unit/Unit.h"
#include "Unit/StatComponent.h"
#include "Unit/StatusEffectComponent.h"

void UEffectManager::ApplyEffect(AUnit* Target, EEffectType Type, int32 Value)
{
	if (!Target || Value <= 0) return;

	UStatusEffectComponent* SEC = Target->FindComponentByClass<UStatusEffectComponent>();
	if (!SEC) return;

	const int32 Current = SEC->GetEffectValue(Type);
	SEC->SetEffectValue(Type, Current + Value);
}

void UEffectManager::ProcessDamage(AUnit* Target, int32 Damage, AUnit* Attacker)
{
	if (!Target || Damage <= 0) return;

	UStatusEffectComponent* SEC = Target->FindComponentByClass<UStatusEffectComponent>();
	if (SEC)
	{
		const int32 Block = SEC->GetEffectValue(EEffectType::Block);
		if (Block > 0)
		{
			const int32 Absorbed = FMath::Min(Block, Damage);
			SEC->SetEffectValue(EEffectType::Block, Block - Absorbed);
			Damage -= Absorbed;
			UE_LOG(LogTemp, Warning, TEXT("[EffectManager] Block 흡수=%d 남은Block=%d 남은Damage=%d"), Absorbed, Block - Absorbed, Damage);
		}
	}

	if (Damage <= 0) return;

	UStatComponent* Stat = Target->GetStat();
	if (Stat)
		Stat->TakeDamage(Damage, Attacker);
}
