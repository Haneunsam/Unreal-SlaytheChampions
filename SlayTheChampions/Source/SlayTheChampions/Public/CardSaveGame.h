#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CardDataTypes.h"
#include "CardSaveGame.generated.h"

/**
 * FPlayerDeckSaveData
 *
 * ��Ƽ�� 1���� �� �����͸� �����ϴ� ����ü.
 * UCardSaveGame �ȿ� ��Ƽ�� ����ŭ �迭�� �����ȴ�.
 * Index 0 = Pawn1, Index 1 = Pawn2
 */
// GameSaveSystem.h 의 FPlayerDeckSaveData 로 대체 예정 - 이름 충돌 방지를 위해 변경
USTRUCT(BlueprintType)
struct FCardSavePlayerDeckData
{
    GENERATED_BODY()

    // 이 파티원의 직업 (Warrior, Mage, Healer 등)
    UPROPERTY(BlueprintReadWrite)
    EJobClass JobClass = EJobClass::Warrior;

    // 카드 ID 배열 (DT_Cards RowName 기준)
    UPROPERTY(BlueprintReadWrite)
    TArray<FName> DeckCards;
};

/**
 * UCardSaveGame
 *
 * ��Ƽ���� �� �����͸� ���Ϸ� ����/�ҷ����� ���.
 *
 * [��� �帧]
 * 1. ���� ���� �� LoadOrCreate() ȣ��
 *    -> SaveGame ���� ������ �ҷ���
 *    -> ������ DT_StarterDeck ���� �о ���� SaveGame ����
 *
 * 2. ���� ���� �� SaveDeckAfterBattle() ȣ��
 *    -> DrawPile + Hand + DiscardPile ���ļ� ���� (A���)
 *
 * 3. ī�� ���� �� AddCard() ȣ��
 *    -> ī�� �߰� �� ��� ����
 *
 * [���� ����]
 * SlotName : "PlayerDeckSave"
 * UserIndex : 0
 */
UCLASS()
class SLAYTHECHAMPIONS_API UCardSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    // ��Ƽ���� �� ������ �迭 (Index 0 = Pawn1, Index 1 = Pawn2)
    UPROPERTY(BlueprintReadWrite, Category = "Save")
    TArray<FCardSavePlayerDeckData> PartyDecks;

    // ���� ���� �̸� (������)
    static const FString SlotName;

    // ���� ���� ���� �ε��� (�̱��÷��� ���� 0 ����)
    static const int32 UserIndex;

    // ����������������������������������������������������������������������������������������������������������������������������������������
    // ���� ���� �Լ�
    // ��𼭵� UCardSaveGame:: ���� �ٷ� ȣ�� ����
    // ����������������������������������������������������������������������������������������������������������������������������������������

    /**
     * SaveGame ������ ��ũ���� �ҷ��´�.
     * ������ ������ nullptr ��ȯ.
     */
    UFUNCTION(BlueprintCallable, Category = "Save")
    static UCardSaveGame* LoadSave();

    /**
     * SaveGame ��ü�� ��ũ�� �����Ѵ�.
     * nullptr �̸� ����.
     */
    UFUNCTION(BlueprintCallable, Category = "Save")
    static void WriteSave(UCardSaveGame* SaveGame);

    /**
     * SaveGame ������ ������ �ҷ�����, ������ DT���� �ʱ� ���� �����Ѵ�.
     * ���� ���� �� 1ȸ ȣ��.
     *
     * @param StarterDeckWarrior  DT_StarterDeck_Warrior (Pawn1 �ʱ� ��)
     * @param StarterDeckMage     DT_StarterDeck_Mage    (Pawn2 �ʱ� ��)
     * @return �ε�/������ UCardSaveGame ��ü
     */
    UFUNCTION(BlueprintCallable, Category = "Save")
    static UCardSaveGame* LoadOrCreate(UDataTable* StarterDeckWarrior, UDataTable* StarterDeckMage);

    /**
     * ���� ���� �� �� ���¸� �����Ѵ�. (A���)
     * DrawPile + Hand + DiscardPile �� ���ļ� DeckCards �� ����.
     * APartyMemberPawn::SaveDeckToSaveGame() ���� ȣ��.
     *
     * @param PawnIndex    ��Ƽ�� �ε��� (0 = Pawn1, 1 = Pawn2)
     * @param DrawPile     ���� ���� ��ο� ����
     * @param Hand         ���� ����
     * @param DiscardPile  ���� ������ ����
     */
    UFUNCTION(BlueprintCallable, Category = "Save")
    static void SaveDeckAfterBattle(int32 PawnIndex,
        const TArray<FName>& DrawPile,
        const TArray<FName>& Hand,
        const TArray<FName>& DiscardPile);

    /**
     * �������� ī�带 ���� �߰��ϰ� ��� �����Ѵ�.
     *
     * @param PawnIndex  ��Ƽ�� �ε���
     * @param CardName   �߰��� ī�� ID (DT_Cards RowName)
     */
    UFUNCTION(BlueprintCallable, Category = "Save")
    static void AddCard(int32 PawnIndex, FName CardName);

    /**
     * ������ ī�带 �����ϰ� ��� �����Ѵ�.
     *
     * @param PawnIndex  ��Ƽ�� �ε���
     * @param CardName   ������ ī�� ID
     */
    UFUNCTION(BlueprintCallable, Category = "Save")
    static void RemoveCard(int32 PawnIndex, FName CardName);

    /**
     * ��Ƽ���� ���� �� ī�� ����� ��ȯ�Ѵ�.
     *
     * @param PawnIndex  ��Ƽ�� �ε���
     * @return ī�� ID �迭
     */
    UFUNCTION(BlueprintPure, Category = "Save")
    static TArray<FName> GetDeckCards(int32 PawnIndex);
};