# 턴 페이즈 구조

## 전체 흐름

```
StartTurn()
  │
  ├─ [DrawPhase]
  │    ├─ ApplyTurnStartEffects(플레이어)   → 플레이어 Shield 리셋, 버프디버프 tick
  │    └─ PlanAllEnemyActions()            → 모든 적 행동 예고 결정 (IntentComponent 갱신)
  │
  ├─ [PlayerActionPhase]
  │    └─ QueuePlayerAction()              → 카드 사용 시 ActionQueue에 적재
  │
  ├─ [PlayerExecutionPhase]
  │    ├─ ExecuteNextAction()              → ActionQueue에서 카드를 하나씩 꺼내 실행
  │    │    └─ ExecuteCard()              → 데미지  회복  Shield 적용
  │    └─ (큐 소진 시) → EnemyPhase 진입
  │
  └─ [EnemyPhase]
       ├─ ApplyTurnStartEffects(적)         → 적 Shield 리셋, 버프디버프 tick
       └─ ExecuteNextEnemyAction() 반복
            ├─ 죽은 적 건너뜀
            ├─ ExecuteEnemyAction()         → PendingAction 기반 실제 행동 실행
            └─ OnEnemyActionComplete()      → 다음 적으로 인덱스 전진
                 └─ 전부 완료 시 → StartTurn() (다음 턴)
```

---

## 페이즈별 상세

### DrawPhase
- `TurnCount` 증가
- 플레이어 Shield 리셋 + 오브젝트형 상태효과 tick (`ApplyTurnStartEffects`)
- 살아있는 모든 적의 `NPCBrainComponentPlanNextAction()` 호출
  - 패턴(Sequential  Weighted)에 따라 이번 턴 행동(`PendingAction`) 결정
  - 단일 타겟은 이 시점에 랜덤 결정되어 `IntentComponent.Current.Target`에 저장
  - UI가 IntentComponent를 읽어 적 머리 위에 행동 예고 표시

### PlayerActionPhase
- 플레이어가 카드를 사용하면 `QueuePlayerAction()` → `ActionQueue`에 적재
- `EndPlayerActionPhase()` 호출 시 PlayerExecutionPhase로 전환

### PlayerExecutionPhase
- `ExecuteNextAction()`  ActionQueue에서 하나씩 꺼내 `ExecuteCard()` 실행
  - Blueprint가 `OnActionExecuted` 수신 후 애니메이션 재생 → 완료 시 `ExecuteNextAction()` 재호출 (현재 [임시] 자동 진행)
- `SkipToEnd()`  남은 큐를 즉시 전부 실행 (애니메이션 스킵)
- 큐 소진 시 `OnExecutionFinished` 브로드캐스트 → EnemyPhase 진입

### EnemyPhase
- 적 Shield 리셋 + 상태효과 tick (`ApplyTurnStartEffects`)
- `ExecuteNextEnemyAction()`  죽은 적 건너뛰며 순서대로 행동 실행
  - DrawPhase에서 정해둔 `PendingAction` 실행
  - 단일 타겟 `IntentComponent.Target` → 무효 시 살아있는 랜덤 플레이어 폴백
  - 광역 타겟 `SpawnedPlayers`  `SpawnedEnemies` 전체 대상
- 모든 적 완료 → `StartTurn()` (다음 턴)

---

## 행동 종류 (EIntentKind)

 Kind  동작  타겟 
---------
 Attack  `ProcessDamage()` × Hits 횟수  TargetType 기준 
 Defend  `ApplyEffect(Shield)`  TargetType 기준 (비어있으면 자신) 
 Buff  미구현 (TODO)  — 
 Debuff  미구현 (TODO)  — 
 NoAttack  행동 없음  — 

---

## 타겟 결정 (적 시점)

 ETargetType  의미 
------
 SingleEnemy  살아있는 랜덤 플레이어 1명 
 AllEnemies  살아있는 모든 플레이어 
 Self  자기 자신 
 SingleAlly  살아있는 랜덤 아군 적 1명 
 AllAllies  살아있는 모든 아군 적 

---

## Shield 리셋 타이밍

 대상  리셋 시점 
------
 플레이어  DrawPhase 시작 
 적  EnemyPhase 시작 

각자의 턴이 시작될 때 Shield가 초기화되어, 상대 턴에서 얻은 Shield는 자신의 턴까지 유지됩니다.

---

## 임시 사항 (추후 수정 예정)

 항목  내용 
------
 플레이어 카드 타겟  `SingleEnemy` 고정 = 적[0] (타겟 선택 UI 미구현) 
... (6줄 남음)