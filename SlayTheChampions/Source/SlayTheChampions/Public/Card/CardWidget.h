#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Card/CardDataTypes.h"
#include "CardWidget.generated.h"

class UCardStyleDataAsset;
class UImage;
class UTextBlock;

/**
 * UCardWidget
 *
 * 카드 한 장 UI 의 C++ 베이스 클래스.
 * WBP_Card 의 부모 클래스로 설정.
 *
 * [위젯 구성]
 * 공통
 *   BorderOuter  : 카드 외부 테두리 이미지
 *   BorderInner  : 카드 내부 이미지
 *   GemImage     : 코스트 젬 이미지
 *
 * 카드마다
 *   MainImage    : 카드 그림
 *
 * 희귀도별
 *   RarityBorder    : 카드 그림 테두리
 *   TypeBackground  : 카드 속성 텍스트 배경
 *
 * 텍스트
 *   CardNameText    : 카드 이름
 *   DescriptionText : 카드 설명
 *   CostText        : 코스트 숫자
 *
 * [사용법]
 * 1. WBP_Card 의 UCardWidget 을 부모로 설정
 * 2. WBP_Card 의 아래 BindWidget 변수명으로 위젯 배치
 * 3. SetCardData() 호출하면 자동으로 내용 채워짐
 */
UCLASS()
class SLAYTHECHAMPIONS_API UCardWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /**
     * 카드 데이터와 스타일을 받아 위젯 전체를 갱신.
     *
     * @param InCardData  FCardDataRow 구조체 (DT_Cards 에서 조회)
     * @param InStyle     직업별 UCardStyleDataAsset (DA_CardStyle_Warrior 등)
     */
    UFUNCTION(BlueprintCallable, Category = "Card|UI")
    void SetCardData(const FCardDataRow& InCardData, UCardStyleDataAsset* InStyle);

    // Blueprint 에서 추가 처리가 필요할 때 오버라이드
    UFUNCTION(BlueprintImplementableEvent, Category = "Card|UI")
    void OnCardDataSet(const FCardDataRow& InCardData);

protected:
    // ── 공통 이미지 (WBP_Card 에서 변수명 일치 배치 필수) ────────────────────

    // 카드 외부 테두리 이미지
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> BorderOuter;

    // 카드 내부 이미지
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> BorderInner;

    // 코스트 젬 이미지
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> GemImage;

    // ── 카드마다 다른 이미지 ─────────────────────────────────────────────────

    // 카드 그림
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> MainImage;

    // ── 희귀도별 이미지 ──────────────────────────────────────────────────────

    // 카드 그림 테두리 (희귀도별 색상 다름)
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> RarityBorder;

    // 카드 속성 텍스트 배경
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> TypeBackground;

    // ── 텍스트 ───────────────────────────────────────────────────────────────

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> CardNameText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> DescriptionText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> CostText;

private:
    // 현재 세팅된 카드 데이터 (Blueprint 에서 읽기 가능)
    UPROPERTY(BlueprintReadOnly, Category = "Card|UI", meta = (AllowPrivateAccess = "true"))
    FCardDataRow CurrentCardData;
};
