# Interface & Camera

## 역할

플레이어의 조작, 정보 전달, 전투 피드백, 카메라 연출을 담당한다.

## 담당 범위

- Battle HUD
- Visual Feedback
- Meta UI
- 파티원 선택과 손패 전환
- 공용 코스트, 상태이상, 적 의도 표시
- 합동기/필살기 카메라 워킹

## 하위 시스템

| 하위 시스템 | 내용 | 완료 기준 |
| --- | --- | --- |
| Battle HUD | 손패, 공용 코스트, 파티원 상태, 적 의도 표시 | 플레이어가 현재 선택 가능한 행동을 읽을 수 있다 |
| Visual Feedback | 타겟팅 연출, 데미지 로그, 합동기/필살기 카메라 | 카드 사용 결과가 시각적으로 구분된다 |
| Meta UI | 스테이지 맵, 덱 요약, 보상 선택 화면 | 전투 밖 진행을 UI로 조작할 수 있다 |

## Phase별 목표

| Phase | 목표 |
| --- | --- |
| Phase 1 | 전투 HUD, 파티원 선택, 카드 사용, 턴 종료 |
| Phase 2 | 3인 파티 UI, 상태이상/버프 표시, 합동기/필살기 피드백 |
| Phase 3 | 스테이지 맵, 보상 선택, 덱 제거 UI |
| Phase 4 | 런 상태, 유물/덱 요약, 보스 경고 표시 |
| Phase 5 | 최소 연출, 카메라, 사운드/이펙트 마감 |

## 참조 문서

- `docs/product-specs/BattleUISystem.md`
- `docs/references/visuals/combat-camera-card-use.md`
