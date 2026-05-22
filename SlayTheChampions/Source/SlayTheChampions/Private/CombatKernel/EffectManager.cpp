#include "CombatKernel/EffectManager.h"
#include "Unit/Unit.h"
#include "Unit/StatComponent.h"
#include "Unit/StatusEffectComponent.h"

// 수치형 효과(Shield 등)를 대상에게 부여. 기존 값에 누적 적용
void UEffectManager::ApplyEffect(AUnit* Target, EEffectType Type, int32 Value)
{
	if (!Target || Value <= 0) return;

	UStatusEffectComponent* SEC = Target->FindComponentByClass<UStatusEffectComponent>();
	if (!SEC) return;

	// 기존 값에 더해서 누적
	const int32 Current = SEC->GetEffectValue(Type);
	SEC->SetEffectValue(Type, Current + Value);
}

// 데미지 파이프라인: Shield로 먼저 흡수하고 남은 수치를 HP에 적용
void UEffectManager::ProcessDamage(AUnit* Target, int32 Damage, AUnit* Attacker)
{
	if (!Target || Damage <= 0) return;

	UStatusEffectComponent* SEC = Target->FindComponentByClass<UStatusEffectComponent>();
	if (SEC)
	{
		const int32 Shield = SEC->GetEffectValue(EEffectType::Shield);
		if (Shield > 0)
		{
			// Shield가 데미지를 흡수. Shield와 Damage 중 작은 값만큼 차감
			const int32 Absorbed = FMath::Min(Shield, Damage);
			SEC->SetEffectValue(EEffectType::Shield, Shield - Absorbed);
			Damage -= Absorbed;
			UE_LOG(LogTemp, Warning, TEXT("[EffectManager] Shield 흡수=%d 남은Shield=%d 남은Damage=%d"), Absorbed, Shield - Absorbed, Damage);
		}
	}

	// Shield로 전부 막힌 경우 HP 감소 없음
	if (Damage <= 0) return;

	UStatComponent* Stat = Target->GetStat();
	if (Stat)
		Stat->TakeDamage(Damage, Attacker);
}
