# TargetResolverSystem.md

## 목적

이 문서는 카드와 Action Event가 어떤 대상을 선택하고, 실행 시점에 대상 유효성을 어떻게 확인하는지 정의한다.

TargetResolverSystem은 카드 사용 시점의 대상 선택과 Action Event 실행 시점의 대상 검증을 분리한다.

## 핵심 원칙

1. 카드는 반드시 TargetType을 가진다.
2. 대상 선택은 카드 사용 시점에 먼저 검증한다.
3. 실제 효과 적용 전, Action Event 실행 시점에 대상 생존 여부를 다시 확인한다.
4. 사망했거나 유효하지 않은 대상은 기본적으로 `Fizzle` 처리한다.
5. 자동 재지정은 MVP에서 제한적으로만 허용한다.

## 대상 타입

| TargetType | 설명 |
| --- | --- |
| Self | 행동 주체 자신 |
| Target Ally | 선택한 아군 1명 |
| All Allies | 모든 아군 |
| Target Enemy | 선택한 적 1마리 |
| All Enemies | 모든 적 |
| All Units | 모든 아군과 모든 적 |
| Field | 필드 또는 전장 상태 |
| Random Enemy | 무작위 적 1마리 |
| Lowest HP Enemy | 체력이 가장 낮은 적 |
| Marked Target | 표식이 있는 대상 우선 |

## 대상 선택 흐름

```text
카드 선택
-> TargetType 확인
-> 선택 가능한 대상 후보 표시
-> 플레이어 대상 선택
-> 대상 유효성 1차 확인
-> Action Event에 TargetIds 저장
```

## 실행 시점 검증

```text
Action Event 실행 시작
-> TargetIds 확인
-> 대상 생존 여부 확인
-> 대상 상태/면역 확인
-> 유효하면 EffectResolverSystem 호출
-> 유효하지 않으면 카드별 처리 규칙 적용
```

## 유효하지 않은 대상 처리

| 처리 | 설명 | MVP 기본 |
| --- | --- | --- |
| Fizzle | 이벤트 무효화 | 기본값 |
| Retarget | 조건에 맞는 다른 대상에게 재지정 | 카드별 예외 |
| Convert | 방어, 드로우 등 다른 효과로 변환 | 특수 카드 예외 |

## 피해 대신 받기와의 관계

피해 대신 받기는 대상 선택 자체를 바꾸는 규칙이 아니라, 피해 적용 직전 EffectResolverSystem에서 피해 수신자를 바꾸는 규칙이다.

즉, TargetResolverSystem은 원래 대상을 확정하고, EffectResolverSystem이 보호/대신 받기 효과를 확인한다.

## 연결 시스템

| 시스템 | 연결 |
| --- | --- |
| `CardSystem.md` | TargetType 정의 |
| `ActionQueueSystem.md` | TargetIds 저장과 실행 시점 검증 |
| `EffectResolverSystem.md` | 최종 효과 적용 |
| `StatusEffectSystem.md` | 표식, 은신, 면역 등 대상 조건 |
| `BattleUISystem.md` | 대상 선택 UI |

## 테스트 기준

- TargetType에 맞는 대상만 선택 가능한가?
- 사망한 대상에게 이벤트가 실행될 때 Fizzle 처리되는가?
- 광역 대상이 살아있는 대상에게만 적용되는가?
- 표식 대상 우선 규칙이 정상 동작하는가?
- 피해 대신 받기와 대상 선택 규칙이 서로 섞이지 않는가?

## 미정 항목

- Retarget을 어떤 카드 타입에 허용할지
- Random Enemy의 난수 시드 처리
- 보스 면역/대상 제한 규칙의 우선순위
