# GameManager.md

## Game Manager System

## 목적

Game Manager System은 게임 전체의 흐름, 일시정지, 옵션, 씬 전환, 전역 상태를 관리하는 상위 시스템이다.

전투, 카드, 적 AI 같은 세부 로직은 각 전용 시스템이 처리하고, Game Manager는 게임의 큰 상태와 공통 기능을 조율한다.

## 핵심 책임

- 게임 시작
- 게임 일시정지
- 게임 재개
- 옵션 메뉴 열기/닫기
- 씬 전환
- 현재 게임 상태 추적
- 입력 모드 전환
- 공통 UI 호출
- 종료 확인
- 최소 저장/로드 진입점 관리

## 관리 범위

Game Manager가 직접 관리하는 것:

- Main Menu
- Start Scene
- Run Start
- Stage Scene
- Battle Scene
- Pause Menu
- Option Menu
- End Scene
- Game Over

Game Manager가 직접 처리하지 않는 것:

- 카드 효과 계산
- 피해/방어 처리
- 적 AI 세부 행동
- 필살기 조건 판정
- 보상 카드 풀 계산
- 개별 UI 위젯 내부 표시 로직

위 기능들은 각 전용 시스템이나 컴포넌트가 처리한다.

## 상태 구조

```text
Boot
-> MainMenu
-> StartScene
-> RunInit
-> StageSelect
-> Battle
-> Reward
-> StageClear
-> EndScene
-> GameOver
```

일시정지는 대부분의 상태에서 끼어들 수 있는 오버레이 상태로 취급한다.

```text
CurrentState
-> Pause
-> Option
-> Resume to CurrentState
```

## Game State

| 상태 | 설명 |
| --- | --- |
| Boot | 게임 초기화 |
| MainMenu | 메인 메뉴 |
| StartScene | 시간이 남을 경우 추가하는 시작 연출 |
| RunInit | 런 데이터 초기화 |
| StageSelect | 스테이지 그리드 선택 |
| Battle | 전투 진행 |
| Reward | 전투 보상 선택 |
| StageClear | 스테이지 클리어 처리 |
| EndScene | 시간이 남을 경우 추가하는 엔딩 연출 |
| GameOver | 패배 또는 런 종료 |
| Pause | 일시정지 |
| Option | 옵션 메뉴 |

## 일시정지 규칙

### 일시정지 가능 상태

- StageSelect
- Battle
- Reward

### 일시정지 시 처리

- 게임 시간 정지 또는 입력 잠금
- 전투 입력 비활성화
- 카드 클릭 비활성화
- Pause Menu 표시
- 현재 상태 저장
- UI 입력 모드로 전환

### 재개 시 처리

- Pause Menu 닫기
- 이전 상태로 복귀
- 전투/스테이지 입력 재활성화
- 게임 입력 모드로 전환

## 옵션 메뉴

옵션 메뉴는 일시정지 메뉴 또는 메인 메뉴에서 접근 가능하다.

### MVP 옵션

| 옵션 | 필요 여부 | 비고 |
| --- | --- | --- |
| 마스터 볼륨 | 필수 | 전체 사운드 |
| BGM 볼륨 | 가능하면 포함 | 시간 부족 시 마스터 볼륨으로 대체 |
| SFX 볼륨 | 가능하면 포함 | 시간 부족 시 마스터 볼륨으로 대체 |
| 해상도 | 보류 가능 | PC 빌드 상황에 따라 |
| 전체화면/창모드 | 가능하면 포함 | 기본 설정만 제공 가능 |
| 언어 | 보류 | 최초 마스터는 한국어 기준 |
| 키 설정 | 보류 | 기본 입력 고정 |

## 입력 모드

| 상황 | 입력 모드 |
| --- | --- |
| 전투 진행 | Game + UI |
| 카드 대상 선택 | Game + UI |
| 일시정지 메뉴 | UI Only |
| 옵션 메뉴 | UI Only |
| 씬 연출 | 입력 제한 또는 Skip만 허용 |

기본 입력:

- `Esc`: 일시정지/뒤로가기
- `Q/E`: 파티원 선택
- `Space`: 턴 종료 또는 확인
- 마우스 클릭: UI 선택, 카드 선택, 대상 선택

## 씬 전환

### 필수 씬

- Main Menu
- Stage/Battle 통합 플레이 씬
- Game Over 또는 Run Result

### 시간이 남으면 추가

- Start Scene
- End Scene

Start/End Scene은 핵심 전투 검증과 직접 관련이 낮으므로, 마감이 앞당겨지면 가장 먼저 제외한다.

## 다른 시스템과의 관계

| 시스템 | 관계 |
| --- | --- |
| Run Manager | 런 시작/종료 요청 |
| Stage Grid Manager | 스테이지 진입/클리어 요청 |
| Battle Manager | 전투 시작/종료 요청 |
| Reward Manager | 보상 화면 진입/종료 요청 |
| UI Layer | 메뉴, 옵션, 일시정지 화면 표시 |
| Save System | 최소 설정값 저장 |

## 결합도 원칙

Game Manager는 각 시스템의 세부 구현을 직접 알지 않는다.

권장 구조:

```text
Game Manager
-> Request Start Battle
-> Battle Manager handles battle
-> Battle Result Event
-> Game Manager changes state
```

피해야 할 구조:

```text
Game Manager
-> Enemy HP 직접 변경
-> 카드 효과 직접 실행
-> 보상 카드 직접 계산
```

## 컴포넌트/클래스 후보

| 이름 | 형태 | 책임 |
| --- | --- | --- |
| UGameFlowSubsystem | GameInstance Subsystem | 전역 상태와 씬 흐름 관리 |
| URunStateSubsystem | GameInstance Subsystem | 런 데이터와 상태 유지 |
| UPauseManagerComponent | Actor Component 또는 Subsystem | 일시정지/재개 처리 |
| UOptionSettingsSubsystem | GameInstance Subsystem | 옵션 값 관리 |
| UInputModeController | UObject 또는 Component | 입력 모드 전환 |
| WBP_PauseMenu | UMG Widget | 일시정지 메뉴 |
| WBP_OptionMenu | UMG Widget | 옵션 메뉴 |

## 최소 구현 범위

P0:

- Main Menu에서 게임 시작
- RunInit 호출
- Stage/Battle 씬 진입
- Esc로 일시정지
- Resume
- Quit to Main Menu
- 기본 옵션 메뉴 열기/닫기

P1:

- 마스터 볼륨 조절
- Game Over 화면
- Run Result 화면

P2:

- Start Scene
- End Scene
- 창모드/전체화면 옵션
- BGM/SFX 분리 볼륨

## 테스트 기준

- 전투 중 Esc를 누르면 Pause Menu가 열린다.
- Pause 상태에서 카드 클릭이나 턴 종료가 실행되지 않는다.
- Resume을 누르면 이전 전투 상태로 돌아간다.
- Option Menu를 열고 닫아도 이전 상태가 유지된다.
- Quit to Main Menu가 전투 상태를 정리한다.
- Game Over 후 새 게임을 시작해도 이전 런 데이터가 섞이지 않는다.

## 리스크

| 리스크 | 설명 | 대응 |
| --- | --- | --- |
| 전역 매니저 비대화 | Game Manager가 모든 시스템을 직접 처리할 위험 | 요청/결과 이벤트 중심으로 제한 |
| 일시정지 버그 | Pause 중 전투 입력이 살아 있을 수 있음 | 입력 모드와 전투 입력 잠금을 분리 |
| 씬 전환 꼬임 | 전투/보상/스테이지 상태가 남을 수 있음 | 상태 전환 시 정리 함수 필수 |
| 옵션 저장 지연 | 저장/로드 고도화는 일정 리스크 | MVP는 설정값 최소 저장만 |

## 미정 항목

- Stage/Battle을 하나의 Persistent Level에서 처리할지, 씬을 분리할지
- Start Scene과 End Scene을 실제 마스터에 포함할지
- 옵션 저장을 파일로 할지, UE 기본 설정 시스템을 사용할지
- Game Manager를 Subsystem 중심으로 둘지, Level Actor 중심으로 둘지

