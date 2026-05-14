# Card & Deck Engine

## 역할

카드 데이터, 카드 조작, 손패, 덱, 버림 pile, 보상 카드 풀을 담당한다.

이 모듈은 카드가 `무엇인지`, `어떤 조건에서 사용할 수 있는지`, `사용하면 어떤 Action Event를 만드는지`, `사용 후 어느 pile로 이동하는지`를 관리한다. Unreal에서는 DataTable 또는 Primary Data Asset 활용을 우선 검토한다.

## 담당 범위

- Card Data
- Card Control
- Hand Control
- Deck Logic
- Reward System
- 카드 등급/코스트/밸류 기준
- 캐릭터별 독립 덱
- 카드 선택 가능 여부 검증
- 카드 사용 요청과 Action Event 생성

## 하위 시스템

| 하위 시스템 | 내용 | 완료 기준 |
| --- | --- | --- |
| Card Data | 데미지, 방어, 상태이상, 코스트, 등급, 대상, 카드 종류 정의 | 카드 데이터만으로 기본 효과가 실행된다 |
| Card Control | 선택한 카드의 사용 가능 여부, 코스트, 소유 캐릭터, 대상 필요 여부를 검증한다 | 사용할 수 없는 카드는 실행 큐에 들어가지 않는다 |
| Hand Control | 드로우, 버리기, 셔플, 손패 제한 관리 | 드로우 pile 부족 시 discard pile을 섞어 추가 드로우한다 |
| Deck Logic | 캐릭터별 독립 덱과 스테이지 간 강화/제거 상태 저장 | 파티원마다 덱/손패/버림 pile이 분리된다 |
| Reward System | 전투 승리 후 카드 풀에서 보상 추첨, 인벤토리/덱 반영 | 보상 카드 선택 후 해당 캐릭터 덱에 반영된다 |

## 카드 조작 책임

카드 조작은 `Card & Deck Engine`이 소유한다.

| 조작 | 책임 |
| --- | --- |
| 카드 선택 | 선택한 카드가 현재 캐릭터 손패에 있는지 확인 |
| 카드 사용 가능 여부 | 코스트, 대상, 상태이상, 카드 제한 조건 확인 |
| 카드 사용 요청 | Combat Kernel에 Action Event 생성 요청 |
| 카드 이동 | 사용 후 discard pile 또는 별도 pile로 이동 |
| 손패 갱신 | 사용/드로우/버림 이후 손패 데이터 갱신 |

UI는 이 결과를 표시하고 입력을 전달할 뿐, 카드 사용 가능 여부와 카드 이동 규칙을 직접 판단하지 않는다.

## Phase별 목표

| Phase | 목표 |
| --- | --- |
| Phase 1 | 캐릭터별 덱, 기본 드로우 5장, 카드 선택/사용 요청, 공용 코스트 사용 |
| Phase 2 | 직업별 카드 15장, 공용 카드 5장, 특수 카드 약 5장 |
| Phase 3 | 전투 후 카드 보상, 덱 압축, 카드 강화/제거 연결 |
| Phase 4 | 보상 카드 풀, 유물/상점/휴식과 덱 상태 연결 |
| Phase 5 | 카드 수치 조정, 등급별 밸류 검수 |

## 참조 문서

- `docs/product-specs/CardSystem.md`
- `docs/product-specs/RewardSystem.md`
- `QUALITY_SCORE.md`
