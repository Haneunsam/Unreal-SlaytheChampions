// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Unit/CombatTypes.h"
#include "StatusEffectmap.generated.h"
class UTexture2D;
/**
 * EEffectType의 기본 동작/UI 메타데이터 매핑 테이블.
 */

USTRUCT(BlueprintType)
struct SLAYTHECHAMPIONS_API FStatusEffectTemplate
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Default")
	int32 DefaultDeltaPerTurn = 0;

    UPROPERTY(EditAnywhere, Category = "Default")
    int32 DefaultFloorValue = MIN_int32;

    UPROPERTY(EditAnywhere, Category = "Default")
    bool bDefaultResetOnTurnStart = false;

    UPROPERTY(EditAnywhere, Category = "UI")
    FText DisplayName;

    UPROPERTY(EditAnywhere, Category = "UI")
    FText Description;

    UPROPERTY(EditAnywhere, Category = "UI")
    UTexture2D* Icon = nullptr;

    UPROPERTY(EditAnywhere, Category = "UI")
    FLinearColor TintColor = FLinearColor::White;
};


UCLASS(BlueprintType)
class SLAYTHECHAMPIONS_API UStatusEffectmap : public UDataAsset
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere,Category = "Status Effects")
    TMap<EEffectType, FStatusEffectTemplate> Templates;

    /** 템플릿 조회. 없으면 nullptr. (C++ 전용) */
    const FStatusEffectTemplate* Resolve(EEffectType Type) const;

    /** 런타임 FStatusEffect 생성. 매핑 없으면 false. */
    UFUNCTION(BlueprintCallable, Category = "Status Effects")
    bool BuildRuntimeEffect(EEffectType Type, int32 Value, int32 Duration, FStatusEffects& OutEffect) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status Effects")
    UTexture2D* GetIcon(EEffectType Type) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Status Effects")
    FText GetDisplayName(EEffectType Type) const;
};
