// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Card/CardDataTypes.h"
#include "CombatTypes.generated.h"

class AUnit;

//���� ����
UENUM(BlueprintType)
enum class ETeam : uint8
{
    Ally    UMETA(DisplayName = "Ally"),
    Enemy   UMETA(DisplayName = "Enemy"),
    Neutral UMETA(DisplayName = "Neutral")
};


//�� �ൿ ������
UENUM(BlueprintType)
enum class EIntentKind : uint8
{
    Attack UMETA(DisplayName = "Attack"),
    Defend UMETA(DisplayName = "Defend"),
    Buff    UMETA(DisplayName = "Buff"),
    Debuff  UMETA(DisplayName = "Debuff"),
    Unknown UMETA(DisplayName = "Unknown")
};

// UI�� �� ����ü�� �о �Ӹ����� �������� ǥ���ϰ� ����
USTRUCT(BlueprintType)
struct SLAYTHECHAMPIONS_API FIntent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly) EIntentKind Kind = EIntentKind::Unknown;
    UPROPERTY(BlueprintReadOnly) int32       Value = 0;  // ���� ������ or ���Ϸ�
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
    Sequential UMETA(DisplayName = "Sequential"), //������� �ݺ�
    Weighted UMETA(DisplayName = "Weighted")//����ġ����
};

// �� �ൿ �� ���� ����. EnemyPatternData �迭�� �����.
USTRUCT(BlueprintType)
struct SLAYTHECHAMPIONS_API FEnemyAction
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly) EIntentKind  IntentKind = EIntentKind::Attack;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) int32        Value = 0;   // ������ or ���Ϸ�
    UPROPERTY(EditAnywhere, BlueprintReadOnly) int32        Hits = 1;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) ETargetType  TargetType = ETargetType::SingleEnemy;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float        Weight = 1.f;   // Weighted ��忡���� ���
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText        DisplayName;
};

UENUM(BlueprintType)
enum class EGimmickTrigger : uint8
{
    HPThresholdBelow UMETA(DisplayName = "HP Below Threshold"), // HP�� X% ����
    TurnCountReached UMETA(DisplayName = "Turn Count Reached"), // N�� ���
    OnDamaged        UMETA(DisplayName = "On Damaged")          // �ǰ� ��
};

USTRUCT(BlueprintType)
struct SLAYTHECHAMPIONS_API FGimmickPhase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName           PhaseName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) EGimmickTrigger Trigger = EGimmickTrigger::HPThresholdBelow;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float           TriggerValue = 0.5f; // HP�� 0~1 ����, Turn�̸� �� ��
    //UPROPERTY(EditAnywhere, BlueprintReadOnly) UEnemyPatternData* SwapToPattern = nullptr;
    //UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<TSubclassOf<UStatusEffect>> ApplyOnEnter;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText           AnnounceText;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool            bOneShot = true; // true�� �� ���� �ߵ�
};
