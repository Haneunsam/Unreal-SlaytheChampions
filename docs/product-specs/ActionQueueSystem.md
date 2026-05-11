# ActionQueueSystem.md

## 목적

이 문서는 카드 사용으로 생성된 Action Event를 저장하고, 턴 종료 후 입력 순서대로 실행하는 규칙을 정의한다.

## 핵심 원칙

1. 카드를 사용하면 Action Event가 생성된다.
2. Action Event는 즉시 실행되지 않고 큐에 저장된다.
3. `InputIndex`가 낮은 이벤트부터 실행한다.
4. 먼저 카드를 사용한 캐릭터가 먼저 행동한다.
5. 큐 실행 결과는 Battle Log에 남긴다.

## Action Event 필드

| 필드 | 설명 |
| --- | --- |
| EventId | 이벤트 고유 ID |
| SourceCardId | 원본 카드 |
| ActorId | 행동 주체 |
| TargetIds | 대상 목록 |
| EventType | Attack/Guard/Skill/Status/Support/Special/Combo/Ultimate |
| EffectPayload | 피해/방어/상태 수치 |
| Tags | 조건 판정 태그 |
| QueueIndex | 큐 등록 순서 |
| InputIndex | 카드 사용 입력 순서 |
| IsUpgradedToUltimate | 필살기 승격 여부 |

## 실행 흐름

```text
카드 사용
-> Action Event 생성
-> InputIndex 부여
-> Action Queue에 등록
-> 턴 종료
-> InputIndex 오름차순 실행
-> 실행 전 대상/상태 유효성 확인
-> UltimateComboSystem 판정
-> EffectResolverSystem 호출
-> Battle Log 기록
```

## 대상 사망 시 처리

MVP 기본값은 `Fizzle`이다.

| 처리 | 설명 |
| --- | --- |
| Fizzle | 이벤트를 무효화한다 |
| Retarget | 조건에 맞는 다른 대상에게 재지정한다 |
| Convert | 방어, 드로우 등 다른 효과로 변환한다 |

자동 재지정은 예측성을 떨어뜨릴 수 있으므로 MVP에서는 카드별 예외로만 허용한다.

## 디버그 표시

행동 큐 UI에는 최소한 아래 정보를 표시한다.

- 실행 순서
- 행동 캐릭터
- 카드 이름
- 대상
- 필살기 승격 가능 여부
- 합동기 패턴 진행도

## 연결 시스템

| 시스템 | 연결 |
| --- | --- |
| `CardSystem.md` | Action Event 생성 |
| `UltimateComboSystem.md` | 필살기/합동기 판정 |
| `EffectResolverSystem.md` | 실제 효과 적용 |
| `BattleUISystem.md` | 큐 미리보기와 Battle Log |

## 테스트 기준

- 카드 사용 순서대로 InputIndex가 부여되는가?
- 턴 종료 후 InputIndex 순서대로 실행되는가?
- 먼저 입력한 캐릭터가 먼저 행동하는가?
- 사망한 대상 이벤트가 MVP 기본값대로 무효화되는가?
- Battle Log로 실행 결과를 추적할 수 있는가?
