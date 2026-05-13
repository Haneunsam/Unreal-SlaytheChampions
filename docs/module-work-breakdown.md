# Module Work Breakdown

## 목적

이 문서는 팀 작업을 Phase 확장 순서가 아니라 모듈 오너십 기준으로 나누기 위한 인덱스다.

Phase는 일정과 마감 단위로 유지하되, 실제 담당 파트는 6개 모듈을 기준으로 배정한다. 각 모듈의 상세 작업 범위, Phase별 목표, 완료 기준은 별도 문서에서 관리한다.

## 운영 원칙

1. 담당자는 Phase가 아니라 모듈을 중심으로 가진다.
2. 각 모듈은 독립 테스트가 가능하도록 컴포넌트, 데이터, 매니저 단위로 분리한다.
3. 모듈 간 연결은 직접 참조보다 이벤트, 인터페이스, 데이터 구조를 우선한다.
4. GitHub Issue 제목은 `[Module][Phase] 작업명` 형식을 사용한다.
5. Notion 작업 보드는 `Module`, `Phase`, `담당자`, `상태`, `완료 기준`을 함께 관리한다.

## 모듈 문서

| 모듈 | 역할 | 문서 |
| --- | --- | --- |
| Combat Kernel | 전투 규칙, 턴 흐름, 액션 실행 엔진 | [combat-kernel.md](module-work-breakdown/combat-kernel.md) |
| Card & Deck Engine | 카드 데이터, 손패, 덱, 보상 카드 풀 | [card-deck-engine.md](module-work-breakdown/card-deck-engine.md) |
| Entity & Stats | 플레이어/적 공통 스탯, 상태이상, 직업 구조 | [entity-stats.md](module-work-breakdown/entity-stats.md) |
| NPC Brain | 적 의도, 몬스터 패턴, 보스 기믹 | [npc-brain.md](module-work-breakdown/npc-brain.md) |
| World Progress | 스테이지 그리드, 런 데이터, 유지/초기화 규칙 | [world-progress.md](module-work-breakdown/world-progress.md) |
| Interface & Camera | 전투 HUD, 카드 조작, 피드백, 카메라 | [interface-camera.md](module-work-breakdown/interface-camera.md) |

## 추천 담당 파트 분배

| 담당 파트 | 추천 담당자 유형 | 병렬 작업 난이도 |
| --- | --- | --- |
| Combat Kernel | 전투 로직을 가장 잘 이해하는 개발자 | 높음 |
| Card & Deck Engine | 데이터 구조와 밸런스 문서를 함께 보는 담당자 | 높음 |
| Entity & Stats | 컴포넌트 구조와 수치 계산을 맡을 담당자 | 보통 |
| NPC Brain | 몬스터 패턴과 전투 재미를 조율할 담당자 | 보통 |
| World Progress | 로그라이크 흐름과 보상 연결을 맡을 담당자 | 보통 |
| Interface & Camera | UI, UX, 카메라, 피드백을 맡을 담당자 | 높음 |
