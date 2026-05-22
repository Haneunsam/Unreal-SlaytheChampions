#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Card/CardDataTypes.h"
#include "CardSubsystem.generated.h"

/**
 * UCardSubsystem
 *
 * GameInstance Subsystem.
 * - DataTable ���� ī�� �����͸� �ε塤ĳ���Ѵ�.
 * - ������ �� Ǯ, ���� ī�� Ǯ ��ȸ�� �����Ѵ�.
 * - ���� ���� ���� ��𼭵� GEngine->GetEngineSubsystem ���� ���� ����.
 *
 * ��� �� (C++):
 *   UCardSubsystem* CS = GetGameInstance()->GetSubsystem<UCardSubsystem>();
 *   const FCardDataRow* Row = CS->GetCard(FName("Warrior_Attack"));
 *
 * ��� �� (Blueprint):
 *   GetGameInstance �� GetSubsystem(CardSubsystem) -> GetCard / GetCardsByClass
 */
UCLASS()
class SLAYTHECHAMPIONS_API UCardSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // ���� Subsystem Lifecycle ����������������������������������������������������������������������������������������������
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // ���� ������ ���̺� ���� ������������������������������������������������������������������������������������������������
    /**
     * ��Ÿ�ӿ� DataTable �� ��ü�ϰų� �ٽ� �ε��� �� ȣ��.
     * Initialize ���� �ڵ����� ȣ��ǹǷ� ���� ���� ȣ���� �ʿ� ����.
     */
    UFUNCTION(BlueprintCallable, Category = "Card|Data")
    void LoadCardDataTable(UDataTable* InTable);

    // ���� ī�� ��ȸ ������������������������������������������������������������������������������������������������������������������

    /**
     * RowName ���� ���� ī�� �����͸� �����´�. ������ nullptr.
     * (const ������ ��ȯ�� UFUNCTION �Ұ� -> C++ ����)
     */
    const FCardDataRow* GetCard(FName RowName) const;

    /** ������ �ش��ϴ� ī�� RowName ��� ��ȯ (Any ���� ī�� ����). */
    UFUNCTION(BlueprintCallable, Category = "Card|Query")
    TArray<FName> GetCardNamesByClass(EJobClass JobClass) const;

    /**
     * ������ �ش��ϴ� FCardDataRow ������ ��� ��ȯ.
     * (const ������ �迭�� UFUNCTION �Ұ� -> C++ ����)
     */
    TArray<const FCardDataRow*> GetCardsByClass(EJobClass JobClass) const;

    /**
     * ���� ī�� Ǯ: ���� + ��͵� ������� �ĺ� ��� ��ȯ.
     * (��͵� ���͸� ���� ��ü�� ���ϸ� Rarity = Normal �ܸ̿� Any �� ó��)
     */
    UFUNCTION(BlueprintCallable, Category = "Card|Reward")
    TArray<FName> GetRewardPool(EJobClass JobClass, ECardRarity MinRarity) const;

    /** ��ü ī�� RowName ��� */
    UFUNCTION(BlueprintCallable, Category = "Card|Query")
    TArray<FName> GetAllCardNames() const;

private:
    // ���� ���� ���� ������������������������������������������������������������������������������������������������������������������
    /** �����Ϳ��� �Ҵ�: Content/Data/DT_Cards */
    UPROPERTY()
    TObjectPtr<UDataTable> CardDataTable;

    /** Rarity ��ġȭ ���� (���� Ǯ ���͸���) */
    static int32 RarityToInt(ECardRarity Rarity);
};