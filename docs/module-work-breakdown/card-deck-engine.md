# Card & Deck Engine

## 역할

카드 데이터, 손패, 덱, 버림 pile, 보상 카드 풀을 담당한다. 데이터 중심 설계가 필요한 모듈이며 Unreal에서는 DataTable 또는 Primary Data Asset 활용을 우선 검토한다.

## 담당 범위

- Card Data
- Hand Control
- Deck Logic
- Reward System
- 카드 등급/코스트/밸류 기준
- 캐릭터별 독립 덱

## 하위 시스템

| 하위 시스템 | 내용 | 완료 기준 |
| --- | --- | --- |
| Card Data | 데미지, 방어, 상태이상, 코스트, 등급, 대상, 카드 종류 정의 | 카드 데이터만으로 기본 효과가 실행된다 |
| Hand Control | 드로우, 버리기, 셔플, 손패 제한 관리 | 드로우 pile 부족 시 discard pile을 섞어 추가 드로우한다 |
| Deck Logic | 캐릭터별 독립 덱과 스테이지 간 강화/제거 상태 저장 | 파티원마다 덱/손패/버림 pile이 분리된다 |
| Reward System | 전투 승리 후 카드 풀에서 보상 추첨, 인벤토리/덱 반영 | 보상 카드 선택 후 해당 캐릭터 덱에 반영된다 |

## Phase별 목표

| Phase | 목표 |
| --- | --- |
| Phase 1 | 캐릭터별 덱, 기본 드로우 5장, 공용 코스트 사용 |
| Phase 2 | 직업별 카드 15장, 공용 카드 5장, 특수 카드 약 5장 |
| Phase 3 | 전투 후 카드 보상, 덱 압축, 카드 강화/제거 연결 |
| Phase 4 | 보상 카드 풀, 유물/상점/휴식과 덱 상태 연결 |
| Phase 5 | 카드 수치 조정, 등급별 밸류 검수 |

## 참조 문서

- `docs/product-specs/CardSystem.md`
- `docs/product-specs/RewardSystem.md`
- `QUALITY_SCORE.md`
