# StatusEffect (버프/디버프/방어도) 설계

## 기본 구조

```cpp
USTRUCT(BlueprintType)
struct FStatusEffect
{
    GENERATED_BODY()

    int32 Value = 0;              // 현재 값
    int32 DeltaPerTurn = 0;       // 턴마다 더해지는 값
    int32 Count = -1;             // 남은 적용 횟수 (-1 = 무한)
    bool bResetOnTurnEnd = false; // 턴 시작 시 강제 0 리셋 (방어도용)
    int32 FloorValue = INT_MIN;   // 최솟값 제한 (디버프=0, 버프=제한없음)
};
```

## 동작 예시

| 종류 | Value | DeltaPerTurn | Count | bResetOnTurnEnd | 결과 |
|------|-------|-------------|-------|-----------------|------|
| 영구 버프 | 3 | 0 | -1 | false | 영구 유지 |
| 일회용 버프 | 5 | -5 | 1 | false | 1턴 후 제거 |
| 증가 버프 | 0 | +1 | -1 | false | 턴마다 1씩 증가 |
| 디버프 | 3 | -1 | -1 | false | 매 턴 -1, 0에서 정지 |
| 방어도 | 5 | 0 | -1 | true | 턴 시작 시 0으로 리셋 |

## 턴 시작 처리 순서

1. `bResetOnTurnEnd == true` → Value = 0 (유물 등으로 조건 변경 가능)
2. `Count > 0` → Value += DeltaPerTurn, Count--
3. `Count == 0` → 효과 제거
4. `Count == -1` → Value += DeltaPerTurn (무한)
5. Value < FloorValue → Value = FloorValue

## 방어도 특이사항

- 기본: 턴 시작 시 전부 리셋 (`bResetOnTurnEnd = true`)
- 유물/카드 확장 시: bResetOnTurnEnd = false로 변경하거나 유지량 상한 추가
  - 예) "방어도 유지" 유물 → bResetOnTurnEnd = false
  - 예) "방어도 최대 10 유지" → bResetOnTurnEnd = false + FloorValue/CapValue 조합

## 미결 사항

- 버프/디버프 타입 구분 방법 (EEffectType enum 추가 여부)
- 효과 소유 위치: CombatManager에서 TMap<AUnit*, TArray<FStatusEffect>> 관리 예정
- FloorValue 대신 타입으로 분기할지 검토 필요
