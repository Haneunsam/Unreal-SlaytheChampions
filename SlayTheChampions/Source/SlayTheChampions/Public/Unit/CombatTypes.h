// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CardDataTypes.h"
#include "CombatTypes.generated.h"

class AUnit;

// 팀 소속
UENUM(BlueprintType)
enum class ETeam : uint8
{
    Ally    UMETA(DisplayName = "Ally"),
    Enemy   UMETA(DisplayName = "Enemy"),
    Neutral UMETA(DisplayName = "Neutral")
};


// 적 행동 의도형
UENUM(BlueprintType)
enum class EIntentKind : uint8
{
    Attack UMETA(DisplayName = "Attack"),
    Defend UMETA(DisplayName = "Defend"),
    Buff    UMETA(DisplayName = "Buff"),
    Debuff  UMETA(DisplayName = "Debuff"),
    Unknown UMETA(DisplayName = "Unknown")
};

// UI에서 이 구조체를 읽어 머리 위에 의도를 표시하게 됨
USTRUCT(BlueprintType)
struct SLAYTHECHAMPIONS_API FIntent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly) EIntentKind Kind = EIntentKind::Unknown;
    UPROPERTY(BlueprintReadOnly) int32       Value = 0;  // 공격 데미지 or 방어력
    UPROPERTY(BlueprintReadOnly) int32       Hits = 1;
    UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<AUnit> Target;
    UPROPERTY(BlueprintReadOnly) FText       DisplayText;
};


//UENUM(BlueprintType)
//enum class ETargetType : uint8
//{
//    SingleEnemy UMETA(DisplayName = "Single Enemy"),
//    ALlEnemies UMETA(DisplayName = "All Enemies"),
//    Self UMETA(Displayname = "Self"),
//    RamdomAlly UMETA(DisplayName = "Random Ally"),
//    AllAlies UMETA(DisplayName = "All Alies")
//};

UENUM(BlueprintType)
enum class EPatternMode : uint8
{
    Sequential UMETA(DisplayName = "Sequential"), // 순차적으로 반복
    Weighted UMETA(DisplayName = "Weighted")       // 가중치 기반
};

// 적 행동 한 단계 정보. EnemyPatternData 배열로 관리됨.
USTRUCT(BlueprintType)
struct SLAYTHECHAMPIONS_API FEnemyAction
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly) EIntentKind  IntentKind = EIntentKind::Attack;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) int32        Value = 0;   // 데미지 or 방어력
    UPROPERTY(EditAnywhere, BlueprintReadOnly) int32        Hits = 1;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) ETargetType  TargetType = ETargetType::SingleEnemy;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float        Weight = 1.f;   // Weighted 모드에서만 사용
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText        DisplayName;
};

UENUM(BlueprintType)
enum class EGimmickTrigger : uint8
{
    HPThresholdBelow UMETA(DisplayName = "HP Below Threshold"), // HP가 X% 이하
    TurnCountReached UMETA(DisplayName = "Turn Count Reached"), // N턴 경과
    OnDamaged        UMETA(DisplayName = "On Damaged")          // 피격 시
};

USTRUCT(BlueprintType)
struct SLAYTHECHAMPIONS_API FGimmickPhase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName           PhaseName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) EGimmickTrigger Trigger = EGimmickTrigger::HPThresholdBelow;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float           TriggerValue = 0.5f; // HP는 0~1 비율, Turn이면 몇 턴
    //UPROPERTY(EditAnywhere, BlueprintReadOnly) UEnemyPatternData* SwapToPattern = nullptr;
    //UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<TSubclassOf<UStatusEffect>> ApplyOnEnter;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText           AnnounceText;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool            bOneShot = true; // true면 한 번만 발동
};
