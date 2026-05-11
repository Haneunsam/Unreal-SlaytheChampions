# CardSystem.md

## 목적

이 문서는 카드 시스템의 데이터 구조, 카드 분류, 등급, 코스트, 효과, 보상 규칙을 정의한다.

카드 시스템은 다른 액터와 직접 강하게 결합하지 않고, `CardData -> ActionEvent -> EffectResolver` 흐름으로 동작한다.

## 핵심 원칙

1. 카드는 직접 피해나 회복을 적용하지 않고 `Action Event`를 만든다.
2. 카드 사용 가능 여부는 공용 코스트, 대상, 소유 파티원, 상태 조건으로 판단한다.
3. 카드 효과는 데이터 기반 `EffectList`로 표현한다.
4. 카드 로직은 특정 캐릭터 액터에 직접 의존하지 않는다.
5. 카드 보상은 직업, 등급, 노드 타입을 기준으로 생성한다.

## 카드 분류

| 분류 | 설명 |
| --- | --- |
| Common Card | 모든 직업이 사용할 수 있는 공용 카드 |
| Class Card | 특정 직업 또는 파티원 역할 전용 카드 |
| Curse Card | 덱과 전투 선택지를 방해하는 카드 |
| Special Card | 적 기믹, 이벤트, 보스전에서 사용 가능한 카드 |

## 카드 등급

| 등급 | 역할 |
| --- | --- |
| Normal | 기본 효과, 덱의 뼈대 |
| Rare | 조건부 시너지, 강한 효율 |
| Legendary | 전략을 바꾸는 강한 효과 또는 고유 기믹 |

카드 밸류 기준은 `QUALITY_SCORE.md`를 따른다.

## 덱과 드로우 규칙

각 캐릭터는 자기 전용 카드 묶음을 가진다.

| 항목 | 규칙 |
| --- | --- |
| 캐릭터별 덱 최소 수 | 1장 |
| 캐릭터별 덱 최대 수 | 10장 |
| 캐릭터별 드로우 최소 수 | 1장 |
| 캐릭터별 드로우 최대 수 | 10장 |
| 기본 드로우 수 | 턴 시작 시 캐릭터마다 5장 |
| 사용한 카드 | 해당 캐릭터의 discard pile로 이동 |

드로우할 카드가 부족하면 해당 캐릭터의 discard pile 전체를 다시 draw pile로 섞은 뒤, 남은 장수만큼 추가 드로우한다.

```text
드로우 필요 장수 확인
-> draw pile에서 가능한 만큼 드로우
-> draw pile이 비었고 아직 더 뽑아야 하면 discard pile을 섞어 draw pile로 이동
-> 남은 장수만큼 추가 드로우
```

draw pile과 discard pile이 모두 비어 있으면 더 이상 드로우하지 않는다.

## 카드 데이터 필드

| 필드 | 설명 |
| --- | --- |
| CardId | 고유 ID |
| DisplayName | 표시 이름 |
| CardClass | Common/Class/Curse/Special |
| Rarity | Normal/Rare/Legendary |
| OwnerClass | 검사/마법사/힐러 등 사용 직업 제한 |
| Cost | 공용 코스트 소비량 |
| EventType | Attack/Guard/Skill/Status/Support/Special/Combo/Ultimate |
| TargetType | Self/Target Ally/All Allies/Target Enemy/All Enemies/All Units/Field |
| EffectList | 생성할 효과 목록 |
| Tags | 조건 판정용 태그 |
| Description | UI 텍스트 |

## 사용 흐름

```text
파티원 선택
-> 선택 파티원의 손패 표시
-> 카드 선택
-> 사용 가능 여부 확인
-> 대상 선택
-> 공용 코스트 소비
-> Action Event 생성
-> Action Queue 등록
-> 카드 discard pile로 이동
```

## Action Event 생성 규칙

카드는 사용 시 아래 정보를 가진 Action Event를 만든다.

| 필드 | 설명 |
| --- | --- |
| SourceCardId | 원본 카드 |
| ActorId | 행동 주체 |
| TargetIds | 대상 목록 |
| EventType | 이벤트 타입 |
| EffectPayload | 피해/방어/상태 수치 |
| Tags | 필살기/합동기 조건 판정용 태그 |
| QueueIndex | 큐 등록 순서 |
| InputIndex | 카드 사용 입력 순서 |

## 컴포넌트 구조

권장 구성:

| 컴포넌트 | 책임 |
| --- | --- |
| DeckComponent | 덱, 손패, 드로우, 버림 pile 관리 |
| CardPlayableComponent | 카드 사용 가능 여부와 사용 요청 처리 |
| ActionQueueComponent | 카드 이벤트 등록과 실행 |
| StatusComponent | 카드 사용 조건에 필요한 상태 확인 |
| CombatStatsComponent | 코스트, HP, Block, Defense Up 버프 등 수치 참조 |

## 테스트 기준

- 카드가 올바른 Action Event를 생성하는가?
- 캐릭터별 덱이 1~10장 제한을 지키는가?
- 턴 시작 시 기본 5장 드로우가 동작하는가?
- 드로우 pile이 부족하면 discard pile을 섞어 추가 드로우하는가?
- 코스트가 부족하면 카드 사용이 막히는가?
- 대상 타입이 올바르게 제한되는가?
- 사용한 카드가 discard pile로 이동하는가?
- 카드 등급과 코스트가 `QUALITY_SCORE.md`의 밸류 기준을 넘지 않는가?
- 특정 직업 전용 카드가 다른 직업 손패에서 사용되지 않는가?

## 미정 항목

- 카드 데이터 최종 저장 방식: Data Asset 또는 Data Table
- 카드 강화 시 Cost/Effect/Rarity 중 어떤 항목이 변하는지
- 저주 카드가 사용 불가 카드인지, 강제 발동 카드인지
- 특수 카드가 덱에 들어가는지, 전투 중 임시 생성되는지
