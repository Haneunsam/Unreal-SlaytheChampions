#include "CombatKernel/EffectManager.h"
#include "Unit/Unit.h"
#include "Unit/StatComponent.h"
#include "Unit/StatusEffectComponent.h"

// ── 내부 헬퍼 ────────────────────────────────────────────────────────────────

// 특정 효과의 스택을 1 감소 (0 미만 불가)
static void TickDownStack(UStatusEffectComponent* SEC, EEffectType Type)
{
	const int32 Val = SEC->GetEffectValue(Type);
	if (Val > 0)
		SEC->SetEffectValue(Type, Val - 1);
}

// ── 효과 부여 ────────────────────────────────────────────────────────────────

// 수치형 효과를 대상에게 누적 부여
// Type == Shield이면 DefenseUp 보너스와 Frail 패널티를 자동으로 반영
void UEffectManager::ApplyEffect(AUnit* Target, EEffectType Type, int32 Value)
{
	if (!Target || Value <= 0) return;

	UStatusEffectComponent* SEC = Target->FindComponentByClass<UStatusEffectComponent>();
	if (!SEC) return;

	if (Type == EEffectType::Shield)
	{
		// DefenseUp: 스택 수만큼 Shield 획득량 추가
		int32 FinalBlock = Value + SEC->GetEffectValue(EEffectType::Buff_DefenseUp);

		// Frail: Shield 획득량 25% 감소
		if (SEC->GetEffectValue(EEffectType::Debuff_Frail) > 0)
			FinalBlock = FMath::FloorToInt(FinalBlock * 0.75f);

		FinalBlock = FMath::Max(0, FinalBlock);
		SEC->SetEffectValue(EEffectType::Shield, SEC->GetEffectValue(EEffectType::Shield) + FinalBlock);

		UE_LOG(LogTemp, Log, TEXT("[EffectManager] Shield +%d applied to %s (total: %d)"),
			FinalBlock, *Target->GetName(), SEC->GetEffectValue(EEffectType::Shield));
		return;
	}

	// 그 외 효과는 기존 값에 누적. 버프/디버프(100~299)는 999 상한
	const int32 NewVal = SEC->GetEffectValue(Type) + Value;
	const uint8 TypeVal = static_cast<uint8>(Type);
	SEC->SetEffectValue(Type, TypeVal >= 100 ? FMath::Min(NewVal, 999) : NewVal);
}

// 버프(100~199) 스택 누적 부여
void UEffectManager::ApplyBuff(AUnit* Target, EEffectType BuffType, int32 Stacks)
{
	const uint8 Val = static_cast<uint8>(BuffType);
	if (Val < 100 || Val > 199)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EffectManager] ApplyBuff: invalid type %d"), Val);
		return;
	}
	ApplyEffect(Target, BuffType, Stacks);
	UE_LOG(LogTemp, Log, TEXT("[EffectManager] Buff %d (+%d stacks) applied to %s"),
		Val, Stacks, *Target->GetName());
}

// 디버프(200~299) 스택 누적 부여
void UEffectManager::ApplyDebuff(AUnit* Target, EEffectType DebuffType, int32 Stacks)
{
	const uint8 Val = static_cast<uint8>(DebuffType);
	if (Val < 200)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EffectManager] ApplyDebuff: invalid type %d"), Val);
		return;
	}
	ApplyEffect(Target, DebuffType, Stacks);
	UE_LOG(LogTemp, Log, TEXT("[EffectManager] Debuff %d (+%d stacks) applied to %s"),
		Val, Stacks, *Target->GetName());
}

// ── 턴 틱 ────────────────────────────────────────────────────────────────────

// 턴 시작 시 유닛의 버프/디버프 처리
// - Regen : 스택 수만큼 HP 회복 후 스택 1 감소
// - Burn  : 스택 수만큼 데미지 후 스택 1 감소
// - Weak / Vulnerable / Frail : 스택 1 감소만
// - AttackUp / DefenseUp : 변경 없음 (영구)
void UEffectManager::TickEffects(AUnit* Unit)
{
	if (!Unit) return;

	UStatusEffectComponent* SEC = Unit->FindComponentByClass<UStatusEffectComponent>();
	if (!SEC) return;

	UStatComponent* Stat = Unit->GetStat();

	// Regen: 회복 후 스택 감소
	const int32 Regen = SEC->GetEffectValue(EEffectType::Buff_Regen);
	if (Regen > 0)
	{
		if (Stat) Stat->Heal(Regen);
		SEC->SetEffectValue(EEffectType::Buff_Regen, Regen - 1);
		UE_LOG(LogTemp, Log, TEXT("[EffectManager] Regen: %s healed %d HP (stacks left: %d)"),
			*Unit->GetName(), Regen, Regen - 1);
	}

	// Burn: 데미지 후 스택 감소
	const int32 Burn = SEC->GetEffectValue(EEffectType::Debuff_Burn);
	if (Burn > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[EffectManager] Burn: %s takes %d damage (stacks left: %d)"),
			*Unit->GetName(), Burn, Burn - 1);
		ProcessDamage(Unit, Burn, nullptr);
		if (Unit->IsAlive())
			SEC->SetEffectValue(EEffectType::Debuff_Burn, Burn - 1);
	}

	// 시간제 스택 감소 (AttackUp / DefenseUp 제외)
	TickDownStack(SEC, EEffectType::Debuff_Weak);
	TickDownStack(SEC, EEffectType::Debuff_Vulnerable);
	TickDownStack(SEC, EEffectType::Debuff_Frail);
}

// ── 데미지 파이프라인 ─────────────────────────────────────────────────────────

// 순서: 공격자 AttackUp(+) → Weak(-25%) → 대상 Vulnerable(+50%) → Shield 흡수 → HP 감소
void UEffectManager::ProcessDamage(AUnit* Target, int32 Damage, AUnit* Attacker)
{
	if (!Target || Damage <= 0) return;

	// 공격자 버프/디버프 반영 (Attacker가 nullptr이면 Burn 등 직접 데미지로 간주, 스킵)
	if (Attacker)
	{
		UStatusEffectComponent* AttSEC = Attacker->FindComponentByClass<UStatusEffectComponent>();
		if (AttSEC)
		{
			// AttackUp: 스택 수만큼 데미지 추가
			Damage += AttSEC->GetEffectValue(EEffectType::Buff_AttackUp);

			// Weak: 데미지 25% 감소
			if (AttSEC->GetEffectValue(EEffectType::Debuff_Weak) > 0)
				Damage = FMath::FloorToInt(Damage * 0.75f);

			Damage = FMath::Max(0, Damage);
		}
	}

	if (Damage <= 0) return;

	UStatusEffectComponent* SEC = Target->FindComponentByClass<UStatusEffectComponent>();
	if (SEC)
	{
		// Vulnerable: 받는 데미지 50% 증가
		if (SEC->GetEffectValue(EEffectType::Debuff_Vulnerable) > 0)
			Damage = FMath::CeilToInt(Damage * 1.5f);

		// Shield 흡수
		const int32 Shield = SEC->GetEffectValue(EEffectType::Shield);
		if (Shield > 0)
		{
			const int32 Absorbed = FMath::Min(Shield, Damage);
			SEC->SetEffectValue(EEffectType::Shield, Shield - Absorbed);
			Damage -= Absorbed;
			UE_LOG(LogTemp, Log, TEXT("[EffectManager] Shield absorbed %d, remaining %d, damage left %d"),
				Absorbed, Shield - Absorbed, Damage);
		}
	}

	if (Damage <= 0) return;

	UStatComponent* Stat = Target->GetStat();
	if (Stat)
		Stat->TakeDamage(Damage, Attacker);
}
