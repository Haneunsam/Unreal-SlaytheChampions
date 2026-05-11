# product-specs

## 목적

이 폴더는 실제 구현 또는 세부 기획으로 바로 이어지는 시스템 단위 문서를 관리한다.

상위 기획은 `docs/game-design/`를 따르고, 수치/규칙은 `docs/mechanics-harness/`를 따른다. 이 폴더의 문서는 그 규칙을 UE5 구현 단위로 나누어 정리한다.

## 문서 목록

| 파일 | 역할 |
| --- | --- |
| `gdd-template.md` | 신규 시스템 기획서 작성 원칙과 코드 설계 기본 원칙 |
| `CardSystem.md` | 카드 데이터, 덱/드로우, 카드 사용, Action Event 생성 |
| `CharacterSystem.md` | 파티원, 직업, 캐릭터 전투 상태, 파티 선택 구조 |
| `CombatSystem.md` | 전투 상태 머신, 턴 진행, 공용 코스트, 전투 종료 |
| `ActionQueueSystem.md` | 카드 입력 순서, Action Event 큐, 실행 순서 |
| `TargetResolverSystem.md` | 카드 대상 타입, 대상 유효성, 사망 대상 처리 |
| `EffectResolverSystem.md` | 피해, Block, Defense Up, 회복, 대상 효과 적용 |
| `StatusEffectSystem.md` | 버프, 디버프, 상태이상, 스택, 지속시간 |
| `UltimateComboSystem.md` | 필살기 자동 승격, 합동기 입력 패턴 |
| `EnemySystem.md` | 적 데이터, 적 AI, 의도 표시, 몬스터/보스 패턴 |
| `RewardSystem.md` | 전투 보상, 카드 보상, 골드, 유물, 포션 |
| `StageGridSystem.md` | 스테이지 그리드, 노드, 스테이지 초기화 |
| `RunSystem.md` | 런 전체 상태, 파티/덱/골드/유물 유지 |
| `BattleUISystem.md` | 전투 UI, 파티원 선택, 손패, 행동 큐, 카메라 레퍼런스 |
| `system-spec-01.md` | Game Manager, 일시정지, 옵션, 씬 전환 관리 |

## 작성 규칙

1. 각 문서는 하나의 시스템 책임만 다룬다.
2. 다른 시스템과의 연결은 입력/출력 또는 이벤트로 적는다.
3. 구체 수치는 `QUALITY_SCORE.md`와 `docs/mechanics-harness/`를 우선한다.
4. 구현 리스크는 `RELIABILITY.md` 또는 `tech-debt-tracker.md`에 연결한다.
5. 문서가 길어지면 하위 시스템 문서로 분리한다.
