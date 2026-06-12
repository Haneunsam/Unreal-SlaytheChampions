# 보스 기믹 UI 인수인계 문서

> 대상: Combat UI 담당자
> 작성자: 스탯/상태이상/직업/의도/패턴/기믹/애니·이펙트 담당
> 목적: 기존 "적 의도(Intent) UI"와 같은 방식으로 **기믹 정보를 화면에 띄우기 위해**,
> 내 쪽(기믹 시스템)이 무엇을 제공하고 UI 쪽이 무엇을 구독/표시하면 되는지 합의한다.
> 상태: **제안(Draft)** — 아래 "확정 필요 체크리스트"를 함께 보고 인터페이스를 확정한다.

---

## 0. 역할 경계 (누가 뭘 만드나)

| 영역 | 담당 | 내용 |
|---|---|---|
| 기믹 데이터/로직 | **나** | `UGimmickComponent` 및 하위 클래스, `FGimmickPhase`, 트리거 평가, 안내/상태 델리게이트 발행 |
| 기믹 → 화면 표시 | **UI 담당** | 머리 위 경고 아이콘, 페이즈 전환 배너, 기믹 전용 게이지(쉴드/DPS 등) 위젯 |
| 데미지·소환 등 효과 실행 | **Combat(전투 오케스트레이션) 담당** | `OnGimmickDamageRequest`, `OnMinionSpawnRequested`, `OnAwakened` 등 구독·실행 |

UI 담당은 **기믹 구체 클래스(`UGimmick_Shield` 등)를 직접 참조하지 않는 것**을 원칙으로 한다.
대신 아래에서 정의하는 **공통 계약(델리게이트/구조체)** 만 구독한다. (직접 참조 최소화 원칙)

---

## 1. 핵심: 기존 의도(Intent) UI 흐름 복습

기믹 UI는 이 흐름에 "얹는" 형태로 설계한다. 먼저 현재 의도가 어떻게 화면까지 가는지 정리한다.

```
DrawPhase
  └ CombatManager::PlanAllEnemyActions()
       ├ Gimmick->OnTurnStart()                  // 기믹 트리거 평가
       ├ NPCBrain->PlanNextAction()              // 이번 턴 행동 결정
       │     └ IntentComponent->SetIntent(FIntent)   // OnIntentChanged 브로드캐스트
       └ MonsterActionWidget->UpdateActions({ IC->Current })  // 머리 위 갱신
```

- **데이터 단위**: `FIntent` (CombatTypes.h)
- **UI 진입점**: `UMonsterActionWidget::UpdateActions(const TArray<FIntent>&)`
- **구독 가능 델리게이트**: `UIntentComponent::OnIntentChanged(const FIntent&)`

> 즉 UI는 이미 매 턴 `FIntent` 한 개(또는 배열)를 받아서 그리고 있다.
> **기믹 경고는 이 `FIntent`에 이미 자리가 마련돼 있다.** (아래 2계층 참고)

---

## 2. UI가 받게 될 기믹 정보 — 3계층

기믹 표현을 난이도/결합도에 따라 3계층으로 나눈다. **1계층부터 붙이면 즉시 효과가 보이고**, 2·3계층은 합의 후 점진 추가한다.

### 계층 1 — 의도에 얹는 "기믹 경고" (이미 자료구조 존재, 최소 작업)

`FIntent`에 이미 다음 필드가 있다:

```cpp
// CombatTypes.h / struct FIntent
bool  bHasGimmickWarning = false;  // 경고 아이콘 표시용
FText GimmickWarningText;           // 경고 문구 (예: "다음 턴 광역 폭발")
```

- **내가 채운다**: `NPCBrain::PlanNextAction`(또는 기믹) 단계에서 이번 턴 위험 예고가 있으면 위 두 필드를 채워 `IntentComponent`에 실어 보낸다.
- **UI가 할 일**: `UpdateActions`로 들어온 `FIntent.bHasGimmickWarning`이 `true`면 의도 아이콘 옆에 ⚠️ 아이콘 + `GimmickWarningText` 툴팁을 추가로 그린다.
- **장점**: 새 델리게이트·새 위젯 없이 `MonsterActionWidget` 안에서만 분기하면 됨.

### 계층 2 — 페이즈 전환 / 즉발 안내 배너 (델리게이트 구독)

기믹이 페이즈로 진입하거나 즉발 연출(각성, 분노, 폭발 예고 등)을 알릴 때 사용한다.

`UGimmickComponent`(베이스)가 제공하는 델리게이트:

```cpp
// GimmickComponent.h — 이미 존재
// 페이즈 진입 시 1회
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseEntered, const FGimmickPhase&, Phase);
UPROPERTY(BlueprintAssignable) FOnPhaseEntered  OnPhaseEntered;

// 텍스트 안내(배너/토스트용)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGimmickAnnounce, const FText&, Text);
UPROPERTY(BlueprintAssignable) FOnGimmickAnnounce OnGimmickAnnounce;
```

`FGimmickPhase`로 넘어오는 표시용 데이터:

```cpp
// CombatTypes.h
FName  PhaseName;     // "Phase2" 등 식별자
FText  AnnounceText;  // 화면에 띄울 문구
// (Trigger / TriggerValue / bOneShot 은 내부 발동조건 — UI는 보통 무시)
```

- **UI가 할 일**:
  - `OnGimmickAnnounce` 구독 → 중앙 배너/토스트로 `Text` 1.5~2초 노출.
  - (선택) `OnPhaseEntered` 구독 → `PhaseName` 기준으로 배경/BGM/이펙트 전환 트리거.
- **현재 상태 주의**: 지금 `CombatManager::HandleGimmickAnnounce`는 `UE_LOG`만 한다. **UI로 가는 경로가 아직 없다.** 이 계층이 이번 작업의 핵심이다.

### 계층 3 — 지속형 기믹 게이지 (제안: 공통 read-model)

쉴드 스택, DPS 체크 누적량, 수면 잔여 턴처럼 **매 턴 갱신되는 숫자**를 머리 위/사이드에 게이지로 보여줄 때 사용한다.
기믹마다 데이터가 제각각이므로, **UI가 구체 클래스를 모르게** 하기 위해 공통 뷰 구조체를 한 개 정의할 것을 제안한다.

```cpp
// (제안) CombatTypes.h 에 추가
UENUM(BlueprintType)
enum class EGimmickGaugeStyle : uint8
{
    None,        // 게이지 없음
    Stack,       // 0/Max 카운터 (쉴드 스택 등)
    Progress,    // 누적/목표 진행바 (DPS 체크 등)
    Countdown    // 잔여 턴 카운트다운 (수면/타이머)
};

USTRUCT(BlueprintType)
struct FGimmickStatusView
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly) FName  GimmickId;   // "Shield","DPSCheck" 등 안정 식별자
    UPROPERTY(BlueprintReadOnly) FText  Label;       // 표시 라벨
    UPROPERTY(BlueprintReadOnly) EGimmickGaugeStyle Style = EGimmickGaugeStyle::None;
    UPROPERTY(BlueprintReadOnly) int32  Current = 0;
    UPROPERTY(BlueprintReadOnly) int32  Max     = 0;
    UPROPERTY(BlueprintReadOnly) bool   bVisible = false;
};

// (제안) UGimmickComponent 베이스에 추가
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGimmickStatusChanged, const FGimmickStatusView&, View);
UPROPERTY(BlueprintAssignable) FOnGimmickStatusChanged OnGimmickStatusChanged;
```

- **내가 할 일**: 각 기믹이 값이 바뀔 때 `OnGimmickStatusChanged`를 브로드캐스트하도록 구현.
- **UI가 할 일**: 적 1명당 `OnGimmickStatusChanged` 1개만 구독 → `Style`에 맞는 게이지 렌더. `bVisible=false`면 숨김.
- **장점**: 쉴드/DPS/수면을 **위젯 하나로 통일**, 신규 기믹 추가 시 UI 수정 불필요.

---

## 3. 기믹별 표시 후보 데이터 표

UI 디자인 시 참고. "현재 노출 경로"가 비어 있으면 내가 계층 3 뷰로 채워줄 항목이다.

| 기믹 | 표시하면 좋은 것 | 권장 계층/스타일 | 현재 노출 경로 |
|---|---|---|---|
| **Shield** | 남은 쉴드 스택 `Current/Max` | 3 / Stack | `OnShieldStackChanged(Old,New)` 존재 (구체 델리게이트) |
| **OverDamage(DPS체크)** | 누적/목표 데미지, 마감까지 잔여 턴 | 3 / Progress | 없음 → 계층3 뷰 필요 |
| **SlowStarter(수면)** | 수면 상태, 각성까지 잔여 턴, 각성 순간 배너 | 3 / Countdown + 2 / 배너 | 각성 시 `OnGimmickAnnounce` 발행 |
| **DeathBomb** | (선택) 죽을 때 터질 누적 데미지 경고 | 1 / 경고 또는 2 / 배너 | 사망 시 `OnGimmickAnnounce` |
| **Summoner** | 소환 예고, 살아있는 하수인 수 | 1 / 경고 + 3 / Stack | `OnMinionSpawnRequested` (소환 실행은 Combat 측) |
| **GimmickData 페이즈** | "페이즈 2 돌입" 배너 | 2 / 배너 | `OnPhaseEntered`, `OnGimmickAnnounce` |

---

## 4. UI 측 적용 예시 (구독 코드 스케치)

> 실제 위젯 구조에 맞춰 조정. 핵심은 "구체 기믹 클래스 대신 베이스 델리게이트만 구독".

```cpp
// 적 1명 위젯 초기화 시
void UEnemyHudWidget::BindToEnemy(AUnit* Enemy)
{
    // 계층 2: 페이즈/안내 배너
    if (UGimmickComponent* G = Enemy->FindComponentByClass<UGimmickComponent>())
    {
        G->OnGimmickAnnounce.AddDynamic(this, &UEnemyHudWidget::ShowBanner);
        G->OnPhaseEntered.AddDynamic(this, &UEnemyHudWidget::OnPhaseEntered);

        // 계층 3: (확정 시) 통합 게이지
        // G->OnGimmickStatusChanged.AddDynamic(this, &UEnemyHudWidget::UpdateGimmickGauge);
    }
}

// 계층 1: 의도에 얹힌 경고는 기존 UpdateActions 안에서 분기
void UMonsterActionWidget::UpdateActions(const TArray<FIntent>& Intents)
{
    // ... 기존 아이콘 생성 ...
    for (const FIntent& I : Intents)
    {
        if (I.bHasGimmickWarning)
        {
            // ⚠️ 경고 아이콘 + I.GimmickWarningText 툴팁 추가
        }
    }
}
```

---

## 5. 현재 미배선 항목 (Combat 담당과 함께 확인 필요)

UI만으로는 못 채우고 **Combat 오케스트레이션 쪽 배선이 끝나야** 화면에 의미가 생기는 것들:

- `OnGimmickAnnounce` → 지금은 `CombatManager`가 `UE_LOG`만 함. **UI로 중계하는 경로 신설 필요.**
- `Gimmick_Shield::ProcessIncomingDamage` → 데미지 파이프라인에서 호출 안 됨. (쉴드 스택 게이지를 그려도 실제 흡수는 아직 미동작)
- `Gimmick_Summoner::OnMinionSpawnRequested` / `RegisterMinion` → 소환 실행부 미배선.
- `Gimmick_SlowStarter::OnAwakened` → 각성 버프 적용 구독자 없음.

> 위 항목은 **로직(나)·실행(Combat)·표시(UI)** 가 모두 닿아야 완성된다. UI는 표시만 책임지되, 위 경로가 살아있는지 합의해두면 "그렸는데 안 변한다" 류 혼선을 막을 수 있다.

---

## 6. 확정 필요 체크리스트

함께 결정한 뒤 인터페이스를 고정하자.

- [ ] **계층 1 경고**: `FIntent.bHasGimmickWarning`/`GimmickWarningText`를 의도 아이콘 옆 어디에 어떻게 표시할지 (아이콘 모양/툴팁 트리거).
- [ ] **계층 2 배너**: `OnGimmickAnnounce` 텍스트를 어떤 위젯(중앙 배너 vs 머리 위 토스트)으로, 노출 시간 얼마로.
- [ ] **계층 2 중계 경로**: `OnGimmickAnnounce`를 UI까지 보내는 방식 — (A) UI가 적 컴포넌트를 직접 구독 vs (B) CombatManager가 받아 UI 전용 델리게이트로 재발행. (B면 Combat 담당 작업 필요)
- [ ] **계층 3 채택 여부**: `FGimmickStatusView` + `OnGimmickStatusChanged` 통합 게이지로 갈지, 기믹별 개별 위젯으로 갈지.
- [ ] 채택 시 **`EGimmickGaugeStyle` 종류**와 각 스타일 위젯 디자인 합의.
- [ ] **갱신 타이밍**: 게이지를 매 프레임 vs 값 변경 시(델리게이트)만 갱신 — 권장은 델리게이트.
- [ ] **다국어/길이**: `AnnounceText`·`Label` 최대 길이 가이드(배너 줄바꿈 정책).

---

## 부록 A — 관련 타입/파일 위치

| 항목 | 파일 |
|---|---|
| `FIntent`, `FGimmickPhase`, `EGimmickTrigger`, `FEnemyAction` | `Unit/CombatTypes.h` |
| 기믹 베이스 + 델리게이트 | `Unit/Enemy/GimmickComponent.h/.cpp` |
| 페이즈 데이터 에셋 | `Unit/Enemy/GimmickData.h` |
| 기믹 부착(데이터 → 런타임) | `Unit/Enemy/EnemyInitializerComponent.cpp` (`InitializeFromDefinition`) |
| 데이터 테이블 | `Unit/Enemy/EnemyDataTable.h` (`FEnemyDefinition.GimmickClass / GimmickData`) |
| 의도 컴포넌트 | `Unit/Enemy/IntentComponent.h/.cpp` |
| 머리 위 의도 위젯 | `CombatKernel/MonsterActionWidget.h/.cpp` |

## 부록 B — 기믹별 노출 델리게이트 요약

```text
UGimmickComponent (베이스)
  OnPhaseEntered(FGimmickPhase)        // 계층2
  OnGimmickAnnounce(FText)             // 계층2
  OnGimmickDamageRequest(...)          // Combat 전용 (UI 무관)
  [제안] OnGimmickStatusChanged(FGimmickStatusView)  // 계층3

UGimmick_Shield
  OnShieldStackChanged(int32 Old, int32 New)   // 쉴드 게이지
UGimmick_Summoner
  OnMinionSpawnRequested(int32 Count)          // 소환(실행은 Combat)
UGimmick_SlowStarter
  OnAwakened()                                 // 각성(버프는 Combat)
```
