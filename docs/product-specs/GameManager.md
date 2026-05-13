# GameManager.md

## Game Manager System

## 목적

Game Manager System은 게임 전체의 전역 실행 상태를 관리하는 최상위 흐름 시스템이다.

Game Manager는 `게임이 실행 중인지`, `정지 상태인지`, `전환 중인지`, `종료 상태인지`만 판단한다. 전투, 런 진행, 옵션 메뉴, 보상, 씬 전환 같은 세부 상태는 각 전담 시스템이 관리한다.

## 핵심 원칙

1. Game Manager는 세부 게임플레이 상태를 직접 소유하지 않는다.
2. Game Manager는 다른 시스템에 요청을 보내고 결과 이벤트를 받는다.
3. Game Manager의 상태는 전역 실행 가능 여부를 판단하는 데만 사용한다.
4. Battle, Reward, Option, StageSelect 같은 상세 상태는 전담 시스템 문서에서 관리한다.

## 전역 상태

| 상태 | 설명 | 예시 |
| --- | --- | --- |
| Boot | 게임 초기화 중 | Subsystem 초기화, 설정 로드 |
| Running | 게임이 정상 실행 중 | 메인 메뉴, 스테이지 선택, 전투, 보상 |
| Paused | 게임 입력/진행이 일시 정지됨 | Pause Menu, Option Menu |
| Transitioning | 씬/레벨/UI 전환 중 | 로딩, 페이드, 씬 교체 |
| GameOver | 런 실패 또는 게임 종료 결과 상태 | 패배 화면, 결과 화면 |
| Exiting | 애플리케이션 종료 처리 중 | Quit 확인 이후 종료 |

## 세부 상태 소유권

| 세부 상태 | 담당 문서 | Game Manager의 역할 |
| --- | --- | --- |
| MainMenu, Loading, StartScene, EndScene | `SceneManagement.md` | 전환 요청 |
| RunInit, StageSelect, StageClear, RunClear, RunFail | `RunSystem.md` | Run 시작/종료 요청 |
| BattleStart, PlayerInput, EnemyTurn, BattleEnd | `CombatSystem.md` | 전투 시작/종료 요청 |
| RewardSelect, RewardApply | `RewardSystem.md` | 보상 진입/종료 요청 |
| PauseMenu, OptionMenu, ConfirmPopup | `CommonUISystem.md` | Paused 전역 상태 적용 |

## 직접 처리하지 않는 것

Game Manager는 아래 작업을 직접 처리하지 않는다.

- 레벨 로드/언로드
- Start Scene, End Scene, Battle Scene 전환
- Pause Menu, Option Menu, Confirm Popup 생성
- Widget Stack 관리
- 입력 모드 전환
- 전투 턴 상태 관리
- 카드 효과 계산
- 피해/방어 처리
- 적 AI 행동
- 보상 카드 풀 계산
- 스테이지 노드 선택 상태 관리

## 전역 상태 전환 흐름

기본 흐름:

```text
Boot
-> Running
-> Transitioning
-> Running
-> Paused
-> Running
-> GameOver
-> Exiting
```

전투나 보상은 Game Manager의 전역 상태를 바꾸지 않는다.

```text
GameManager: Running
RunSystem: StageSelect
CombatSystem: None
CommonUISystem: None
```

전투 진입 예시:

```text
GameManager: Running
RunSystem: BattleNode
CombatSystem: BattleStart -> PlayerInput -> EnemyTurn
SceneManagement: GameplayScene active
```

옵션 메뉴 예시:

```text
GameManager: Paused
CommonUISystem: OptionMenu
CombatSystem: PlayerInput 유지, 입력 잠금
```

씬 전환 예시:

```text
GameManager: Transitioning
SceneManagement: Loading -> Applying -> Complete
CommonUISystem: Loading UI
```

## 요청/이벤트 구조

Game Manager는 직접 처리 대신 요청 이벤트를 발행한다.

```text
RequestStartRun
-> RunSystem initializes run data
-> SceneManagement opens gameplay scene
-> GameManager enters Running
```

```text
RequestPause
-> GameManager enters Paused
-> CommonUISystem opens PauseMenu
-> Input system locks gameplay input
```

```text
RequestResume
-> CommonUISystem closes top overlay
-> Input system restores previous mode
-> GameManager enters Running
```

```text
RequestSceneTransition
-> GameManager enters Transitioning
-> SceneManagement handles transition
-> SceneTransitionComplete
-> GameManager enters Running
```

## Pause / Resume 규칙

Pause는 세부 상태가 아니라 전역 실행 상태다.

Pause 가능 여부는 Game Manager가 판단하되, 실제 UI와 입력 처리는 Common UI System과 Input System이 담당한다.

### Pause 가능 조건

- Game Manager 상태가 `Running`
- Scene Management 상태가 `Idle` 또는 `Complete`
- Common UI에 Modal 전환이 진행 중이지 않음

### Pause 요청 시 Game Manager 책임

- 이전 전역 상태 저장
- 전역 상태를 `Paused`로 변경
- `CommonUISystem.OpenPauseMenu` 요청
- `InputModeController.LockGameplayInput` 요청

### Resume 요청 시 Game Manager 책임

- 전역 상태를 `Running`으로 변경
- `CommonUISystem.CloseTopOverlay` 요청
- `InputModeController.RestorePreviousInputMode` 요청

## 다른 시스템과의 관계

| 시스템 | 관계 |
| --- | --- |
| Scene Management | Transitioning 상태에서 씬 전환을 수행한다 |
| Common UI System | Paused 상태에서 Pause/Option/Confirm UI를 관리한다 |
| Run System | Running 상태 안에서 런 진행 상태를 관리한다 |
| Combat Kernel | Running 상태 안에서 전투 상태를 관리한다 |
| Reward System | Running 상태 안에서 보상 상태를 관리한다 |
| Save System | 설정값과 최소 진행 상태 저장 요청을 받는다 |

## 컴포넌트/클래스 후보

| 이름 | 형태 | 책임 |
| --- | --- | --- |
| UGameFlowSubsystem | GameInstance Subsystem | 전역 실행 상태 관리 |
| UGameFlowRouter | UObject | 요청 라우팅과 전환 가능 여부 검증 |
| EGlobalGameState | Enum | Boot, Running, Paused, Transitioning, GameOver, Exiting |
| FGameFlowRequest | Struct | 전역 상태 전환 요청 데이터 |


## 리스크

| 리스크 | 설명 | 대응 |
| --- | --- | --- |
| 전역 매니저 비대화 | Game Manager가 Battle/Reward/Option 상태까지 소유할 위험 | 전역 상태만 허용 |
| 상태 중복 | RunSystem과 GameManager가 같은 상태를 중복 관리할 수 있음 | 전역 상태와 세부 상태를 문서상 분리 |
| Pause 입력 버그 | Paused 상태에서 전투 입력이 살아 있을 수 있음 | Common UI와 Input Lock을 분리 관리 |
| 전환 중 중복 요청 | Transitioning 중 다른 요청이 들어올 수 있음 | 요청 큐 또는 요청 거절 규칙 도입 |
