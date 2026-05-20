#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardDataTypes.h"
#include "CardWidget.generated.h"

class UCardStyleDataAsset;
class UImage;
class UTextBlock;

/**
 * UCardWidget
 *
 * ī�� �� �� ���� C++ ���̽� Ŭ����.
 * WBP_Card �� �θ� Ŭ������ ���.
 *
 * [���� ����]
 * ������
 *   BorderOuter  : ī�� �ܺ� �׵θ� ���
 *   BorderInner  : ī�� ���� ���
 *   GemImage     : �»�� ����
 *
 * ī�帶��
 *   MainImage    : ī�� �׸�
 *
 * ��͵���
 *   RarityBorder    : ī�� �׸� �׵θ�
 *   TypeBackground  : ī�� �Ӽ� �ؽ�Ʈ ���
 *
 * �ؽ�Ʈ
 *   CardNameText    : ī�� �̸�
 *   DescriptionText : ī�� ����
 *   CostText        : �ڽ�Ʈ ����
 *
 * [����]
 * 1. WBP_Card �� UCardWidget �θ�� ����
 * 2. WBP_Card �� �Ʒ� BindWidget �������� ������ ���� ��ġ
 * 3. SetCardData() ȣ���ϸ� �ڵ����� ���� ä����
 */
UCLASS()
class SLAYTHECHAMPIONS_API UCardWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /**
     * ī�� �����Ϳ� ���� ��Ÿ���� �޾� ���� ��ü�� ����.
     *
     * @param InCardData  FCardDataRow ������ (DT_Cards ���� ��ȸ)
     * @param InStyle     ������ UCardStyleDataAsset (DA_CardStyle_Warrior ��)
     */
    UFUNCTION(BlueprintCallable, Category = "Card|UI")
    void SetCardData(const FCardDataRow& InCardData, UCardStyleDataAsset* InStyle);

    // Blueprint ���� �߰� ó���� �ʿ��� �� �������̵�
    UFUNCTION(BlueprintImplementableEvent, Category = "Card|UI")
    void OnCardDataSet(const FCardDataRow& InCardData);

protected:
    // ���� ������ �̹��� (WBP_Card �� ������ �̸����� ���� ��ġ �ʼ�) ����������������

    // ī�� �ܺ� �׵θ� ���
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> BorderOuter;

    // ī�� ���� ���
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> BorderInner;

    // �»�� ����
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> GemImage;

    // ���� ī�帶�� �ٸ� �̹��� ��������������������������������������������������������������������������������������������

    // ī�� �׸�
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> MainImage;

    // ���� ��͵��� �̹��� ������������������������������������������������������������������������������������������������������

    // ī�� �׸� �׵θ�
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> RarityBorder;

    // ī�� �Ӽ� �ؽ�Ʈ ���
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> TypeBackground;

    // ���� �ؽ�Ʈ ������������������������������������������������������������������������������������������������������������������������

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> CardNameText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> DescriptionText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> CostText;

private:
    // ���� ���õ� ī�� ������
    UPROPERTY(BlueprintReadOnly, Category = "Card|UI", meta = (AllowPrivateAccess = "true"))
    FCardDataRow CurrentCardData;
};