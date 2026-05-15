# SlayTheChampions - 작업 로그

## 프로젝트 개요
- **장르**: 카드게임 (Slay the Spire 스타일 배틀 시스템)
- **엔진**: Unreal Engine (Blueprint → C++ 전환 중)
- **목표**: Slay the Spire 방식의 턴제 카드 배틀 시스템 구현

## 현재 C++ 구현 현황

### 완료된 파일
| 파일 | 설명 |
|------|------|
| `GameManagers/GameManager` | 게임 상태 관리 (GameInstanceSubsystem), EGameState (Run/Pause/Transition/Exit) |
| `GameManagers/LevelManager` | 씬 전환 관리 |
| `Map/Area` | 맵 영역 기초 |
| `Map/MapCreator` | 맵 생성 |
| `Map/MapManager` | 맵 관리 |
| `Map/MapEnum` | 맵 관련 Enum |
| `Map/MapStruct` | 맵 관련 Struct |

### 미구현 (예정)
- [ ] 배틀 시스템
  - [ ] BattleManager (턴 진행, 페이즈 관리)
  - [ ] Card / CardData (카드 데이터, 효과)
  - [ ] DeckManager (덱, 손패, 버림패)
  - [ ] Enemy / EnemyAI (적 행동 패턴)
  - [ ] Player / PlayerStats (HP, 방어도, 에너지)
  - [ ] StatusEffect (버프/디버프)
  - [ ] BattleUI 연동

---

## 작업 기록

### 2026-05-15
**시작 시 상태**: Map 시스템 및 GameManager 기초 완료. 배틀 시스템 미구현.
**작업 내용**:
- WORK_LOG.md 작성 (작업 추적 시스템 세팅)
- 프로젝트 구조 파악 완료
- 버그 수정:
  - `Area.h` — `TArray<UArea>` → `TArray<UArea*>` (UObject 포인터 규칙)
  - `Area.h` — `SetNextAreas` 파라미터 타입도 동일하게 수정
  - `MapCreator.h` — 순수 C++ 클래스에 있던 `UPROPERTY` 매크로 제거
  - `MapCreator.h` — `class Area*` → `class UArea*` 포워드 선언 수정
  - `MapCreator.h` — `GridMap.SetNum` → `GridMap.Init(0, ...)` (0으로 초기화 보장)
  - `MapCreator.h` — 깨진 한글 주석 정리
  - `MapCreator.cpp` — `GridMapCreate()` 구현 (규칙 7개: 고정행 배치, 랜덤 노드 배치, 열 최소 보장, 연결 수 제한)

**다음 작업 예정**:
- 배틀 시스템 설계 및 C++ 구현 시작

---
<!-- 새 작업 기록은 위 형식으로 아래에 추가 -->
