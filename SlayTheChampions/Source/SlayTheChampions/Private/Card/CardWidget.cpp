#include "Card/CardWidget.h"
#include "Card/CardStyleDataAsset.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PaperSprite.h"

/**
 * PaperSprite 를 UImage 위젯에 적용하는 내부 헬퍼 함수.
 * Sprite 가 nullptr 이면 무시.
 *
 * @param ImageWidget  적용할 UImage 위젯
 * @param Sprite       적용할 UPaperSprite
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
    // 현재 카드 데이터 저장 (Blueprint 에서 읽기 가능)
    CurrentCardData = InCardData;

    if (InStyle)
    {
        // 공통 이미지 적용 (희귀도 무관) ────────────────────────────────────
        // 카드 외부 테두리, 내부 이미지, 코스트 젬 이미지 적용
        ApplySpriteToImage(BorderOuter, InStyle->GetBorderOuter());
        ApplySpriteToImage(BorderInner, InStyle->GetBorderInner());
        ApplySpriteToImage(GemImage, InStyle->GetGemImage());

        // 희귀도별 이미지 적용 ────────────────────────────────────────────────
        // 카드 그림 테두리, 속성 텍스트 배경을 희귀도에 맞게 적용
        ApplySpriteToImage(RarityBorder, InStyle->GetRarityBorder(InCardData.Rarity));
        ApplySpriteToImage(TypeBackground, InStyle->GetTypeBackground(InCardData.Rarity));
    }

    // 카드마다 다른 그림 적용 ─────────────────────────────────────────────────
    // DT_Cards 의 MainImage 에서 해당 카드 스프라이트 동기 로드
    ApplySpriteToImage(MainImage, InCardData.MainImage.LoadSynchronous());

    // 텍스트 적용 ─────────────────────────────────────────────────────────────
    if (CardNameText)
        CardNameText->SetText(InCardData.CardName);

    if (DescriptionText)
        DescriptionText->SetText(InCardData.Description);

    if (CostText)
        CostText->SetText(FText::AsNumber(InCardData.Cost));

    // Blueprint 에서 추가 처리가 필요할 때 호출
    OnCardDataSet(InCardData);
}
