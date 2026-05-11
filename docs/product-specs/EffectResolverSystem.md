# EffectResolverSystem.md

## 목적

이 문서는 Action Event의 실제 효과를 전투 상태에 적용하는 규칙을 정의한다.

피해, Block, Defense Up, 회복, 상태이상, 필드 효과는 모두 EffectResolverSystem을 통해 처리한다.

## 처리 순서

```text
사망/행동 불가 확인
-> 대상 유효성 확인
-> 보호/피해 대신 받기 확인
-> Block 확인
-> Defense Up 버프 확인
-> 피해 증가/감소 확인
-> 효과 적용
-> 사망 처리
-> 후속 트리거 처리
```

## 피해 계산

```text
카드 피해 = 카드 기본값 + 공격자 Power
실제 체력 피해 = max(0, 카드 피해 - 대상 Block - 대상 Defense Up)
```

`Defense Up`은 기본 방어력 스탯이 아니라 버프다.

## 지원 효과 처리

| 효과 | 처리 |
| --- | --- |
| Block 획득 | 대상의 Block 증가 |
| Defense Up | 상태 버프로 부여 |
| 회복 | HP 증가, MaxHP 초과 불가 |
| 상태 부여 | StatusEffectSystem에 요청 |
| 드로우 | CardSystem/DeckComponent에 요청 |
| 코스트 회복 | CombatSystem에 요청 |

## 피해 대신 받기

```text
원래 피해 대상 확인
-> 피해 대신 받기 상태 확인
-> 대신 받을 파티원으로 대상 변경
-> 대신 받는 파티원의 Block과 Defense Up 기준으로 피해 계산
```

## 연결 시스템

| 시스템 | 연결 |
| --- | --- |
| `ActionQueueSystem.md` | 실행할 Action Event 수신 |
| `StatusEffectSystem.md` | 버프/디버프 적용과 참조 |
| `CharacterSystem.md` | 파티원 HP/Block 변경 |
| `EnemySystem.md` | 적 HP/Block 변경 |
| `CardSystem.md` | EffectPayload 정의 |

## 테스트 기준

- Block이 체력 피해보다 먼저 차감되는가?
- Defense Up이 실제 체력 피해를 수치만큼 줄이는가?
- 회복이 MaxHP를 넘지 않는가?
- 상태 부여/제거가 StatusEffectSystem을 통해 처리되는가?
- 피해 대신 받기가 대신 받는 캐릭터의 방어 기준으로 계산되는가?

## 미정 항목

- Block 초과 피해 처리의 세부 연출
- 피해 증가/감소의 곱연산/합연산 순서
- 광역 피해의 대상별 순차 처리 여부
