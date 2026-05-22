#include "Card/CardWidget.h"
#include "Card/CardStyleDataAsset.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PaperSprite.h"

/**
 * PaperSprite �� UImage ������ �����ϴ� ���� ���� �Լ�.
 * Sprite �� nullptr �̸� ����.
 *
 * @param ImageWidget  ������ UImage ����
 * @param Sprite       ������ UPaperSprite
 */
static void ApplySpriteToImage(UImage* ImageWidget, UPaperSprite* Sprite)
{
    if (!ImageWidget || !Sprite) return;

    FSlateBrush Brush;
    Brush.SetResourceObject(Sprite);
    ImageWidget->SetBrush(Brush);
}

void UCardWidget::SetCardData(const FCardDataRow& InCardData, UCardStyleDataAsset* InStyle)
{
    // ���� ī�� ������ ���� (Blueprint ���� ��ȸ ����)
    CurrentCardData = InCardData;

    if (InStyle)
    {
        // ���� ������ �̹��� ���� (��͵� ����) ����������������������������������������������������������
        // ī�� �ܺ� �׵θ�, ���� ���, ������ �������� ����
        ApplySpriteToImage(BorderOuter, InStyle->GetBorderOuter());
        ApplySpriteToImage(BorderInner, InStyle->GetBorderInner());
        ApplySpriteToImage(GemImage, InStyle->GetGemImage());

        // ���� ��͵��� �̹��� ���� ����������������������������������������������������������������������������������
        // ī�� �׸� �׵θ�, �Ӽ� �ؽ�Ʈ ����� ��͵��� ���� ����
        ApplySpriteToImage(RarityBorder, InStyle->GetRarityBorder(InCardData.Rarity));
        ApplySpriteToImage(TypeBackground, InStyle->GetTypeBackground(InCardData.Rarity));
    }

    // ���� ī�� �׸� ���� (ī�帶�� �ٸ�) ����������������������������������������������������������������������
    // DT_Cards �� MainImage ���� �ش� ī�� ��������Ʈ �ε�
    ApplySpriteToImage(MainImage, InCardData.MainImage.LoadSynchronous());

    // ���� �ؽ�Ʈ ���� ������������������������������������������������������������������������������������������������������������
    if (CardNameText)
        CardNameText->SetText(InCardData.CardName);

    if (DescriptionText)
        DescriptionText->SetText(InCardData.Description);

    if (CostText)
        CostText->SetText(FText::AsNumber(InCardData.Cost));

    // Blueprint ���� �߰� ó���� �ʿ��� �� ȣ��
    OnCardDataSet(InCardData);
}