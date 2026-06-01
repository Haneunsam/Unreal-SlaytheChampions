# DT_Cards.csv 카드 효과 수정 튜토리얼

---

## 1. CSV 컬럼 구조

| 컬럼 | 타입 | 설명 |
|------|------|------|
| RowName | FName | 카드 고유 키 (예: `Warrior_Attack`) |
| CardID | int32 | 카드 번호 (101, 201 ...) |
| CardName | FText | UI 표시용 이름 |
| Description | FText | UI 표시용 설명 |
| MainImage | TSoftObjectPtr | 카드 이미지 경로 |
| Rarity | ECardRarity | `Normal` / `Rare` / `Legendary` |
| CardType | ECardType | `Attack` / `Skill` |
| RequiredClass | EJobClass | `Warrior` / `Mage` / `Healer` / `Any` |
| Cost | int32 | 사용 코스트 |
| Damage | int32 | 기본 데미지 |
| Block | int32 | 기본 방어도 |
| DrawCount | int32 | 드로우 카드 수 |
| UsingCount | int32 | 효과 반복 횟수 (기본 1) |
| HealAmount | int32 | 회복량 |
| **Effects** | TArray\<FCardEffect\> | **버프/디버프 효과 목록** |
| TargetType | ETargetType | 효과 대상 |
| ScoreValue | int32 | 밸런스 점수 (기획용) |
| Notes | FString | 메모 |

---

## 2. Effects 컬럼 작성 규칙

각 효과는 `(EffectType=...,Value=...,TargetType=...)` 형태입니다.

| 상황 | 작성법 |
|------|--------|
| 효과 없음 | `()` |
| 효과 1개 | `"((EffectType=Debuff_Weak,Value=2,TargetType=UseCardDefault))"` |
| 효과 2개 이상 | `"((EffectType=Debuff_Weak,Value=2,TargetType=SingleEnemy),(EffectType=Buff_AttackUp,Value=1,TargetType=Self))"` |

> ⚠️ **효과가 1개 이상이면 전체를 `"큰따옴표"`로 감싸야 합니다.**
> CSV에서 쉼표가 컬럼 구분자로 오인되기 때문입니다.

### TargetType 옵션 (효과별 독립 대상)

| TargetType | 대상 |
|-----------|------|
| `UseCardDefault` | 카드의 기본 TargetType 그대로 사용 **(기본값)** |
| `SingleEnemy` | 선택한 적 1명 |
| `AllEnemies` | 모든 적 |
| `Self` | 시전자 본인 |
| `SingleAlly` | 선택한 아군 1명 |
| `AllAllies` | 모든 아군 |

---

## 3. EffectType 목록

### 즉발 효과 (0~99)

| EffectType | Value 의미 | 동작 |
|-----------|-----------|------|
| `Shield` | 방어도 수치 | 즉시 방어도 +Value |
| `Heal` | 회복량 | 즉시 HP +Value |
| `Damage` | 데미지 | 즉시 데미지 Value |

### 버프 (100~199) — 매 턴 스택 감소 없음

| EffectType | Value 의미 | 동작 |
|-----------|-----------|------|
| `Buff_AttackUp` | 스택 수 | 공격 데미지 +스택 수 (영구 유지) |
| `Buff_DefenseUp` | 스택 수 | Shield 획득량 +스택 수 (영구 유지) |
| `Buff_Regen` | 스택 수 | 매 턴 시작 시 스택 수만큼 HP 회복 → 스택 1 감소 |

### 디버프 (200~299) — 매 턴 스택 1 감소

| EffectType | Value 의미 | 동작 |
|-----------|-----------|------|
| `Debuff_Weak` | 지속 턴 | 공격 데미지 25% 감소 |
| `Debuff_Vulnerable` | 지속 턴 | 받는 데미지 50% 증가 |
| `Debuff_Burn` | 스택 수 | 매 턴 시작 시 스택 수만큼 데미지 → 스택 1 감소 |
| `Debuff_Frail` | 지속 턴 | Shield 획득량 25% 감소 |

---

## 4. TargetType — 효과 대상

| TargetType | 대상 |
|-----------|------|
| `SingleEnemy` | 플레이어가 선택한 적 1명 |
| `AllEnemies` | 모든 적 |
| `Self` | 카드를 사용한 플레이어 본인 |
| `SingleAlly` | 플레이어가 선택한 아군 1명 |
| `AllAllies` | 모든 아군 |
| `Single_Team` | 선택한 아군 1명 |

> Damage, Block, Effects 모두 동일한 TargetType 대상에게 적용됩니다.

---

## 5. 카드 예시

### 5-1. 순수 데미지 카드

| RowName | CardID | CardName | Cost | Damage | Block | Effects | TargetType |
|---------|--------|----------|------|--------|-------|---------|-----------|
| Warrior_Attack | 101 | 타격 | 1 | 6 | 0 | `()` | SingleEnemy |

---

### 5-2. 데미지 + 디버프 (약화 공격)

| RowName | CardID | CardName | Cost | Damage | Block | Effects | TargetType |
|---------|--------|----------|------|--------|-------|---------|-----------|
| Warrior_Weaken | 105 | 약화 공격 | 1 | 6 | 0 | `"((EffectType=Debuff_Weak,Value=2))"` | SingleEnemy |

**동작:** 적에게 6 데미지 + 약화 2턴 부여 (약화 중 적 공격력 25% 감소)

---

### 5-3. 디버프만 (화염)

| RowName | CardID | CardName | Cost | Damage | Block | Effects | TargetType |
|---------|--------|----------|------|--------|-------|---------|-----------|
| Warrior_Burn | 106 | 화염 | 1 | 0 | 0 | `"((EffectType=Debuff_Burn,Value=3))"` | SingleEnemy |

**동작:** 적에게 화상 3 부여 (매 턴 시작 시 3 데미지, 이후 스택 1 감소)

---

### 5-4. 버프 2개 동시 (전투 준비)

| RowName | CardID | CardName | Cost | Damage | Block | Effects | TargetType |
|---------|--------|----------|------|--------|-------|---------|-----------|
| Warrior_PowerUp | 107 | 전투 준비 | 1 | 0 | 0 | `"((EffectType=Buff_AttackUp,Value=2),(EffectType=Buff_DefenseUp,Value=1))"` | Self |

**동작:** 공격력 +2, 방어력 +1 영구 획득

---

### 5-5. 방어 + 적 취약 부여 (도발)

| RowName | CardID | CardName | Cost | Damage | Block | Effects | TargetType |
|---------|--------|----------|------|--------|-------|---------|-----------|
| Warrior_Taunt | 108 | 도발 | 1 | 0 | 4 | `"((EffectType=Debuff_Vulnerable,Value=2))"` | SingleEnemy |

**동작:** 방어도 4 획득 + 적에게 취약 2턴 부여 (취약 중 받는 데미지 50% 증가)

---

### 5-6. 아군 재생 부여 (치유사)

| RowName | CardID | CardName | Cost | Damage | Block | Effects | TargetType |
|---------|--------|----------|------|--------|-------|---------|-----------|
| Healer_Regen | 303 | 재생 | 1 | 0 | 0 | `"((EffectType=Buff_Regen,Value=3))"` | SingleAlly |

**동작:** 아군 1명에게 재생 3 부여 (매 턴 3 회복, 이후 스택 1 감소)

---

### 5-7. 다중 타격 + 화상 (불꽃 연격)

| RowName | CardID | CardName | Cost | Damage | Block | UsingCount | Effects | TargetType |
|---------|--------|----------|------|--------|-------|-----------|---------|-----------|
| Warrior_FlameStrike | 109 | 불꽃 연격 | 2 | 4 | 0 | 2 | `"((EffectType=Debuff_Burn,Value=2))"` | SingleEnemy |

**동작:** 4 데미지 2회 + 화상 2 부여

---

## 6. UE 에디터 적용 방법

```
1. DT_Cards.csv 수정 후 저장
2. UE 에디터 → Content Browser → 04_Data → DT_Cards 우클릭
3. Reimport
4. Ctrl+S 저장
```

---

## 7. 자주 하는 실수

| 실수 | 원인 | 해결 |
|------|------|------|
| Effects 컬럼이 여러 칸으로 쪼개짐 | `"` 없이 쉼표 포함 | 효과 1개 이상이면 `"..."` 로 전체 감싸기 |
| 에디터에서 효과가 적용 안 됨 | Reimport 누락 | DT_Cards 우클릭 → Reimport |
| EffectType 인식 실패 | 오타 또는 대소문자 틀림 | 위 표의 이름 그대로 복사 |
| Value=0 효과가 무시됨 | 코드에서 `Value <= 0` skip | Value는 반드시 1 이상 |
| 버프가 적에게 들어감 | TargetType 설정 오류 | 아군 버프는 `Self` 또는 `SingleAlly` 사용 |
