#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CardDataTypes.h"
#include "PaperSprite.h"
#include "CardStyleDataAsset.generated.h"

/**
 * UCardStyleDataAsset
 *
 * 직업별 카드 스타일 이미지를 보관하는 DataAsset.
 *
 * [직업별 - 희귀도 무관]
 *  BorderOuter  : 카드 외부 테두리 배경
 *  BorderInner  : 카드 내부 배경
 *  GemImage     : 좌상단 보석 이미지
 *
 * [희귀도별 - Normal/Rare/Legendary]
 *  RarityBorder     : 카드 그림 테두리
 *  TypeBackground   : 카드 속성 텍스트 배경
 *
 * [생성 방법]
 * Content Browser -> Miscellaneous -> Data Asset -> CardStyleDataAsset 검색
 * DA_CardStyle_Warrior, DA_CardStyle_Mage, DA_CardStyle_Healer 생성 후 이미지 지정
 */
UCLASS(BlueprintType)
class SLAYTHECHAMPIONS_API UCardStyleDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    // 이 스타일이 어떤 직업용인지
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CardStyle")
    EJobClass JobClass = EJobClass::Any;

    // ── 직업별 이미지 (희귀도 무관) ──────────────────────────────────────

    // 카드 외부 테두리 배경
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CardStyle|Job")
    TSoftObjectPtr<UPaperSprite> BorderOuter;

    // 카드 내부 배경
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CardStyle|Job")
    TSoftObjectPtr<UPaperSprite> BorderInner;

    // 좌상단 보석 이미지
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CardStyle|Job")
    TSoftObjectPtr<UPaperSprite> GemImage;

    // ── 희귀도별 이미지 ───────────────────────────────────────────────────

    // 카드 그림 테두리 (Normal)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CardStyle|Rarity")
    TSoftObjectPtr<UPaperSprite> RarityBorder_Normal;

    // 카드 그림 테두리 (Rare)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CardStyle|Rarity")
    TSoftObjectPtr<UPaperSprite> RarityBorder_Rare;

    // 카드 그림 테두리 (Legendary)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CardStyle|Rarity")
    TSoftObjectPtr<UPaperSprite> RarityBorder_Legendary;

    // 카드 속성 텍스트 배경 (Normal)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CardStyle|Rarity")
    TSoftObjectPtr<UPaperSprite> TypeBackground_Normal;

    // 카드 속성 텍스트 배경 (Rare)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CardStyle|Rarity")
    TSoftObjectPtr<UPaperSprite> TypeBackground_Rare;

    // 카드 속성 텍스트 배경 (Legendary)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CardStyle|Rarity")
    TSoftObjectPtr<UPaperSprite> TypeBackground_Legendary;

    // ── 헬퍼 함수 ────────────────────────────────────────────────────────

    // 직업별 이미지 반환
    UFUNCTION(BlueprintCallable, Category = "CardStyle")
    UPaperSprite* GetBorderOuter() const { return BorderOuter.LoadSynchronous(); }

    UFUNCTION(BlueprintCallable, Category = "CardStyle")
    UPaperSprite* GetBorderInner() const { return BorderInner.LoadSynchronous(); }

    UFUNCTION(BlueprintCallable, Category = "CardStyle")
    UPaperSprite* GetGemImage() const { return GemImage.LoadSynchronous(); }

    // 희귀도에 맞는 카드 그림 테두리 반환
    UFUNCTION(BlueprintCallable, Category = "CardStyle")
    UPaperSprite* GetRarityBorder(ECardRarity Rarity) const;

    // 희귀도에 맞는 카드 속성 텍스트 배경 반환
    UFUNCTION(BlueprintCallable, Category = "CardStyle")
    UPaperSprite* GetTypeBackground(ECardRarity Rarity) const;
};