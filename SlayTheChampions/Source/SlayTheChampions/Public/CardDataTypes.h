#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PaperSprite.h"
#include "CardDataTypes.generated.h"

/**
 * ECardRarity
 * ī�� ��͵� ���.
 * DA_CardStyle ���� ��͵��� �̹����� �����ϴ� �� ���.
 */
UENUM(BlueprintType)
enum class ECardRarity : uint8
{
    Normal    UMETA(DisplayName = "Normal"),
    Rare      UMETA(DisplayName = "Rare"),
    Legendary UMETA(DisplayName = "Legendary"),
    Status    UMETA(DisplayName = "Status"),
};

/**
 * ECardType
 * ī�� ����.
 * ����/���/��ų �� ī���� ������ ����.
 */
UENUM(BlueprintType)
enum class ECardType : uint8
{
    Attack  UMETA(DisplayName = "Attack"),
    Defense UMETA(DisplayName = "Defense"),
    Skill   UMETA(DisplayName = "Skill"),
    Buff    UMETA(DisplayName = "Buff"),
    AOE     UMETA(DisplayName = "AOE"),
    Status  UMETA(DisplayName = "Status"),
};

/**
 * EJobClass
 * ���� ����.
 * ī�尡 � ���� ��������, DA_CardStyle ���� ���п� ���.
 */
UENUM(BlueprintType)
enum class EJobClass : uint8
{
    Any     UMETA(DisplayName = "Any"),
    Warrior UMETA(DisplayName = "Warrior"),
    Mage    UMETA(DisplayName = "Mage"),
    Healer  UMETA(DisplayName = "Healer"),
};

/**
 * ETargetType
 * ī�� ��� Ÿ��.
 * ���� ��/��ü ��/�ڽ�/�Ʊ� �� ī�� ȿ�� ���� ��� ����.
 */
UENUM(BlueprintType)
enum class ETargetType : uint8
{
    SingleEnemy UMETA(DisplayName = "SingleEnemy"),  // �� ���� ���
    AllEnemies  UMETA(DisplayName = "AllEnemies"),   // �� ��ü
    Self        UMETA(DisplayName = "Self"),          // �ڱ� �ڽ�
    SingleAlly  UMETA(DisplayName = "SingleAlly"),   // �Ʊ� ���� ���
    AllAllies   UMETA(DisplayName = "AllAllies"),     // �Ʊ� ��ü
    Single_Team UMETA(DisplayName = "Single_Team"),  // ��Ƽ�� ����
};

/**
 * FCardDataRow
 *
 * DT_Cards DataTable �� Row Struct.
 * ī�� 1���� ��� ������ ��´�.
 *
 * [����]
 * - MainImage  : PaperSprite (pixelCardAssest_Sprite_XX)
 * - CardSubsystem ���� RowName(ī�� ID) ���� ��ȸ
 * - HandComponent ���� FName(ī�� ID) ���� ����
 * - CardWidget �� SetCardData() �� �����ؼ� UI ����
 */
USTRUCT(BlueprintType)
struct FCardDataRow : public FTableRowBase
{
    GENERATED_BODY()

    // ���� �ĺ� ��������������������������������������������������������������������������������������������������������������������������

    // 카드 고유 ID (DT_Cards의 RowName과 매핑, ex: Warrior_Attack)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Identity")
    FName CardID;

    // ī�� �̸� (Ÿ��, ���� ��) - UI ǥ�ÿ�
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Identity")
    FText CardName;

    // ī�� ���� �ؽ�Ʈ - UI ǥ�ÿ�
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Identity")
    FText Description;

    /**
     * ī�� ���� �̹��� (PaperSprite).
     * ī�� 1�帶�� �ٸ� ��������Ʈ ����.
     * DT_Cards ���� pixelCardAssest_Sprite_XX ����.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Identity")
    TSoftObjectPtr<UPaperSprite> MainImage;

    // ���� �з� ��������������������������������������������������������������������������������������������������������������������������

    // ��͵� (DA_CardStyle �� ��͵��� �̹��� ���ÿ� ���)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Category")
    ECardRarity Rarity = ECardRarity::Normal;

    // ī�� Ÿ�� (����/��ų ��)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Category")
    ECardType CardType = ECardType::Attack;

    // ��� ���� ���� (Any �� �� ���� ��� ����)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Category")
    EJobClass RequiredClass = EJobClass::Any;

    // ���� �ڽ�Ʈ ����������������������������������������������������������������������������������������������������������������������

    // ī�� ��� �ڽ�Ʈ (��Ƽ ���� �ڽ�Ʈ���� ����)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Cost",
        meta = (ClampMin = "0", ClampMax = "10"))
    int32 Cost = 1;

    // ���� ȿ�� ��ġ ������������������������������������������������������������������������������������������������������������������

    // ���ط�
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Effect",
        meta = (ClampMin = "0"))
    int32 Damage = 0;

    // ��
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Effect",
        meta = (ClampMin = "0"))
    int32 Block = 0;

    // ��ο� ��
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Effect",
        meta = (ClampMin = "0"))
    int32 DrawCount = 0;

    // ��� Ƚ�� (1 = 1ȸ, 2 = 2ȸ ���)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Effect",
        meta = (ClampMin = "1"))
    int32 UsingCount = 1;

    // ȸ����
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Effect",
        meta = (ClampMin = "0"))
    int32 HealAmount = 0;

    // ���� Ư�� ȿ�� ������������������������������������������������������������������������������������������������������������������

    // Ư�� ȿ�� �±� (����/����� ID ��)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Effect")
    FName EffectTag;

    // Ư�� ȿ�� ��ġ
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Effect",
        meta = (ClampMin = "0"))
    int32 EffectValue = 0;

    // ���� ��� ��������������������������������������������������������������������������������������������������������������������������

    // ī�� ȿ�� ���� ���
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Target")
    ETargetType TargetType = ETargetType::SingleEnemy;

    // ���� �뷱�� ����������������������������������������������������������������������������������������������������������������������

    // ī�� �뷱�� ���� (��ȹ ������)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Balance",
        meta = (ClampMin = "0"))
    int32 ScoreValue = 0;

    // ���� ��Ÿ ��������������������������������������������������������������������������������������������������������������������������

    // ��ȹ �޸� (��Ÿ�� �̻��)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Meta")
    FString Notes;
};