# ARCHITECTURE.md

## 목적

이 문서는 UE5에서 파티 기반 로그라이크 덱빌딩 게임을 구현하기 위한 기술 아키텍처와 모듈 구조를 정의한다.

목표는 2026-06-11 1차 플레이어블 마스터까지 핵심 전투 루프를 안정적으로 검증할 수 있는 구조를 만드는 것이다.

## 아키텍처 원칙

1. 전투 규칙은 데이터 중심으로 만든다.
2. 카드 효과는 하드코딩보다 `Action Event` 생성 규칙으로 표현한다.
3. 행동 큐, 대상 판정, 필살기 자동 승격은 C++ 또는 안정적인 코어 로직으로 관리한다.
4. 카드, 직업, 적, 상태이상, 보상 데이터는 Data Asset/Data Table 기반으로 확장한다.
5. UI와 연출은 블루프린트/UMG에서 빠르게 반복한다.
6. 한 달 마스터 일정에서는 복잡한 엔진 시스템보다 검증 가능한 전투 루프를 우선한다.
7. 기능은 가능한 한 컴포넌트 단위로 분리해 다른 액터에서도 재사용할 수 있게 만든다.
8. 클래스 간 직접 결합을 줄이고 테스트 가능한 단위로 나눈다.

## 전체 모듈 구조

```text
GameInstance / Save
-> Run Manager
-> Stage Grid Manager
-> Battle Manager
   -> Party Manager
   -> Enemy Manager
   -> Card Manager
   -> Action Queue
   -> Effect Resolver
   -> Status Manager
   -> Ultimate/Combo Trigger Manager
-> Reward Manager
-> UI Layer
```

## 핵심 런타임 흐름

```text
스테이지 시작
-> Stage Grid Manager가 노드 생성
-> 노드 선택
-> Battle Manager가 전투 시작
-> Party Manager가 파티원별 덱/손패 구성
-> Card Manager가 카드 사용 요청 처리
-> Action Queue에 Action Event 등록
-> 턴 종료
-> Action Queue 순차 실행
-> Effect Resolver가 피해/방어/상태 처리
-> Ultimate/Combo Trigger Manager가 필살기 자동 승격과 합동기 조건 판정
-> Enemy Manager가 적 AI 실행
-> 전투 종료
-> Reward Manager가 보상 처리
-> 스테이지 클리어 시 스테이지 한정 상태 초기화
```

## 주요 시스템

### 1. Run Manager

런 전체 상태를 관리한다.

**책임:**
- 현재 Act/스테이지 진행 상태 관리
- 파티 구성 유지
- 덱, 유물, 골드, 영구 보상 관리
- 스테이지 시작/클리어/런 종료 처리

**유지 범위:**

| 데이터 | 유지 범위 |
| --- | --- |
| 파티 구성 | 런 전체 |
| 파티원 덱 | 런 전체 |
| 유물/장비 | 런 전체 |
| 골드 | 런 전체 |
| 필살기 조건 스택 | 스테이지 내부 |
| 손패/방어도 | 전투 내부 |

### 2. Stage Grid Manager

그리드 방식의 스테이지 노드 진행을 관리한다.

**책임:**
- 스테이지 노드 생성
- 선택 가능한 다음 노드 계산
- 노드 타입 결정
- 노드 결과를 Battle/Reward/Event/Rest/Shop 시스템에 연결
- 스테이지 클리어 시 스테이지 한정 상태 초기화 요청

**노드 타입:**
- Battle
- Elite
- Boss
- Event
- Rest
- Shop
- Recruit

### 3. Battle Manager

전투의 최상위 상태 머신이다.

**전투 상태:**

```text
BattleStart
-> PlayerTurnStart
-> PlayerCardSelect
-> PlayerActionResolve
-> EnemyTurn
-> TurnCleanup
-> BattleEnd
```

**책임:**
- 현재 턴과 진영 상태 관리
- 공용 코스트 초기화/소비
- 파티원 선택 상태 관리
- 플레이어 턴 종료 처리
- 적 턴 시작 처리
- 승리/패배 조건 판정

### 4. Party Manager

파티원 데이터와 파티원별 전투 상태를 관리한다.

**책임:**
- 검사/마법사/힐러 초기 파티 구성
- 파티원별 HP, Block, Status, Defense Up 버프 관리
- 파티원별 덱, 드로우 pile, discard pile, hand 관리
- 캐릭터별 덱 1~10장 제한 관리
- 캐릭터별 기본 드로우 5장 처리
- 드로우 pile 부족 시 discard pile을 섞어 추가 드로우
- 파티원 선택 요청 처리
- 피해 대신 받기 대상 판정 지원

**최초 직업 3종:**

| 직업 | 역할 | 시스템 요구 |
| --- | --- | --- |
| 검사 | 단일 공격/강화 | 강화 스택, 공격 필살기 자동 승격 |
| 마법사 | 광역 피해/필드 효과 | 원소반응, 필드 효과, 광역 대상, 디버프 |
| 힐러 | 회복/보호/버프 | 회복, 정화, 아군 지정, 전체 보호 |

### 5. Card Manager

카드 사용 가능 여부와 Action Event 생성을 관리한다.

**책임:**
- 선택 파티원의 손패 표시 데이터 제공
- 카드 코스트 확인
- 공용 코스트 소비
- 대상 선택 가능 여부 확인
- 카드 타입에 따라 Action Event 생성
- Instant Effect와 Action Event 구분
- 사용한 카드를 discard pile로 이동

**카드 데이터 최소 필드:**

| 필드 | 설명 |
| --- | --- |
| CardId | 고유 ID |
| DisplayName | 표시 이름 |
| CardClass | Common/Class/Curse/Special |
| Rarity | Normal/Rare/Legendary |
| OwnerClass | 검사/마법사/힐러 등 제한 |
| Cost | 공용 코스트 소비량 |
| EventType | Attack/Guard/Skill/Status/Support/Special/Combo/Ultimate |
| TargetType | Self/Target Ally/All Allies/Target Enemy/All Enemies/All Units/Field |
| EffectList | 생성할 효과 목록 |
| Tags | 강화, 화염, 회복 등 조건 태그 |
| Description | UI 텍스트 |

### 6. Action Queue

플레이어가 사용한 카드 이벤트를 순서대로 저장하고 실행한다.

**책임:**
- 카드 사용 순서대로 Action Event 등록
- 턴 종료 후 큐 순차 실행
- 이벤트 실행 전 대상 생존 여부 확인
- 이벤트 실행 시 필살기/합동기 판정 요청
- 이벤트 실행 결과를 Battle Log로 기록

**Action Event 최소 필드:**

| 필드 | 설명 |
| --- | --- |
| EventId | 이벤트 고유 ID |
| SourceCardId | 원본 카드 |
| ActorId | 행동 파티원 |
| TargetIds | 대상 목록 |
| EventType | 이벤트 타입 |
| EffectPayload | 피해/방어/상태 수치 |
| Tags | 조건 판정용 태그 |
| QueueIndex | 실행 순서 |
| InputIndex | 카드 사용 입력 순서 |
| IsUpgradedToUltimate | 필살기 자동 승격 여부 |

### 7. Effect Resolver

피해, 방어, 회복, 상태이상, 필드 효과를 실제 전투 상태에 적용한다.

**책임:**
- 피해 계산
- Block 차감
- Defense Up 버프 기반 피해 감소
- HP 변경
- 방어도 획득
- 회복 처리
- 상태이상 부여/제거
- 모든 유닛 대상 처리
- 필드 효과 적용

**처리 순서:**

```text
사망/행동 불가 확인
-> 보호/피해 대신 받기 확인
-> 피해 증가/감소 확인
-> 필살기/합동기 조건 확인
-> 효과 적용
-> 사망 처리
-> 후속 트리거 처리
```

### 8. Status Manager

상태이상과 필살기 조건 스택을 관리한다.

**책임:**
- Buff/Debuff/Control/Trigger/Curse 관리
- 필살기 조건 스택 관리
- 스테이지 내부 유지 상태와 전투 내부 상태 구분
- 스테이지 클리어 시 필살기 조건 스택 초기화
- UI에 상태 표시 데이터 제공

### 9. Ultimate/Combo Trigger Manager

필살기 자동 승격과 합동기 입력 패턴 조건을 판정한다.

**책임:**
- Action Event 실행 시점에 조건 검사
- Battle Log와 Status Stack 참조
- 조건 충족 시 필살기는 EventType을 Ultimate로 승격
- 합동기는 InputIndex 기반 입력 패턴을 확인해 소규모 보너스 적용
- 스택 소비 규칙 적용
- 전용 연출 요청 이벤트 발행

**기본 규칙:**
- 플레이어가 별도 필살기 버튼을 누르지 않는다.
- 조건을 만족한 카드 이벤트가 실행 시점에 자동 승격된다.
- 합동기는 필살기와 분리된 작은 시너지 보너스로 처리한다.
- 필살기 조건 스택은 스테이지 내부에서 유지된다.
- 스테이지 클리어 시 초기화된다.

### 10. Enemy Manager

적 배치와 AI 행동을 관리한다.

**책임:**
- 적 데이터 생성
- 적 HP/Block/Status/Defense Up 버프 관리
- 적 의도 표시
- 플레이어 행동 종료 후 적 AI 실행
- 대상 선택
- 보스/엘리트 패턴 처리

**MVP AI 단계:**

1. 고정 패턴
2. 랜덤 가중치 패턴
3. 조건부 패턴

최초 마스터에서는 1~2단계까지만 우선한다.

### 11. Reward Manager

전투 종료 후 보상을 관리한다.

**책임:**
- 카드 보상 3택 1 생성
- 직업별 보상 풀 참조
- 공용 카드/직업 전용 카드 비율 조정
- 카드 등급별 보상 풀과 노드 타입별 보상 기준 적용
- 골드/유물/회복 보상 처리
- 보상 결과를 Run Manager에 반영

## UI 구조

### 전투 UI

**필수 요소:**
- 파티원 3명 상태 패널
- 선택 파티원 표시
- 선택 파티원의 손패
- 비선택 파티원 손패 요약
- 파티 공용 코스트
- 적 의도
- 행동 큐 미리보기
- 턴 종료 버튼
- 상태이상/필살기 조건 스택 표시

### 조작

| 입력 | 기능 |
| --- | --- |
| Q/E | 이전/다음 파티원 선택 |
| 마우스 클릭 | 파티원 선택, 카드 선택, 대상 선택 |
| Space 또는 버튼 | 턴 종료 |
| Esc | 취소/메뉴 |

### UI 원칙

- 선택 파티원의 손패를 가장 크게 보여준다.
- 비선택 파티원의 손패는 카드 수, 주요 태그, 사용 가능 코스트 정도만 요약한다.
- 행동 큐는 카드 사용 순서를 확인할 수 있어야 한다.
- 필살기 조건 스택은 우연히 터졌다고 느껴지지 않도록 진행도를 보여준다.
- 카드 사용 화면의 카메라/전투 UI 레퍼런스는 `docs/references/visuals/combat-camera-card-use.md`를 따른다.

## 데이터 구조 권장

### Data Asset 후보

| 데이터 | 형태 | 설명 |
| --- | --- | --- |
| CardData | Primary Data Asset | 카드 기본 정보 |
| CharacterClassData | Primary Data Asset | 직업/파티원 정보 |
| EnemyData | Primary Data Asset | 적 기본 정보 |
| StatusData | Primary Data Asset | 상태이상 정보 |
| UltimateRuleData | Primary Data Asset | 필살기 조건과 승격 효과 |
| ComboRuleData | Primary Data Asset | 합동기 입력 패턴 조건 |
| DeckRuleData | Data Asset 또는 Config | 캐릭터별 덱 크기, 드로우 수 제한 |
| StageNodeData | Data Table 또는 Data Asset | 노드 타입과 보상 |
| RewardPoolData | Data Asset | 보상 카드 풀 |

### C++ 우선 구현

- Battle Manager
- Action Queue
- Effect Resolver
- Target Resolver
- Ultimate/Combo Trigger Manager
- Card Data 구조체
- Status Stack 구조체

### Blueprint/UMG 우선 구현

- 전투 화면 UI
- 카드 위젯
- 파티원 패널
- 적 패널
- 행동 큐 미리보기
- 연출 트리거
- 스테이지 그리드 화면
- 카드 사용 화면 카메라 레퍼런스 적용

## 최소 프로토타입 구현 순서

1. 파티원 2명과 적 1마리를 화면에 배치한다.
2. 파티원별 손패와 공용 코스트를 만든다.
3. 카드를 클릭하면 Action Event가 생성되게 한다.
4. 행동 큐 UI에 사용 순서를 표시한다.
5. 턴 종료 시 큐를 순차 실행한다.
6. 피해/방어/회복을 Effect Resolver로 처리한다.
7. 적이 플레이어 행동 후 공격하게 한다.
8. 검사 강화 3스택 후 공격 자동 승격을 구현한다.
9. 파티원 3명으로 확장한다.
10. 스테이지 그리드와 보상 흐름을 연결한다.

## MVP 기술 범위

### 반드시 구현

- 파티원별 덱/손패
- 캐릭터별 기본 드로우와 discard pile 재순환
- 공용 코스트
- 카드 사용과 Action Event 생성
- 행동 큐 실행
- 대상 선택
- 피해/방어/회복
- 상태이상 기본 구조
- 필살기 자동 승격
- 합동기 조건 판정
- 적 AI 기본 행동
- 전투 보상
- 스테이지 내부 상태 유지/초기화

### 보류

- 복잡한 GAS 기반 능력 시스템
- 네트워크 멀티플레이
- 고급 저장/로드
- 복잡한 물리 연출
- 완성형 시네마틱
- 절차적 맵 생성 고도화

## 주요 기술 리스크

| 리스크 | 설명 | 대응 |
| --- | --- | --- |
| 카드 효과 확장성 | 카드가 늘어날수록 조건 분기가 복잡해짐 | 효과를 EffectList와 태그 기반으로 구성 |
| 행동 큐 디버깅 | 이벤트 순서, 대상, 필살기 승격이 꼬일 수 있음 | Battle Log를 남기고 큐 상태를 UI로 표시 |
| UI 복잡도 | 파티원 3명의 손패와 상태를 한 화면에 담아야 함 | 선택 파티원 중심 UI, 비선택 파티원 요약 |
| 필살기 조건 판정 | 전투/스테이지 범위 상태가 섞일 수 있음 | Status Scope를 Combat/Stage/Run으로 분리 |
| 콘텐츠 제작 비용 | 직업별 덱 때문에 카드 수가 빠르게 증가 | 최초 마스터는 검사/마법사/힐러만 구현 |
