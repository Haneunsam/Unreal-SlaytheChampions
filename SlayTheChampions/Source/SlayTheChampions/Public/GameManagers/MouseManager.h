// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "MouseManager.generated.h"

UCLASS()
class SLAYTHECHAMPIONS_API UMouseManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    /*레벨 전환시 마우스 커서의 주인 설정*/
    UFUNCTION(BlueprintCallable, Category = "Mouse Manager")
    void SetupMouseCursor(APlayerController* PC, TSubclassOf<UUserWidget> CursorWidgetClass);

    // 마우스 커서 보이기 / 숨기기 통합 관리
    UFUNCTION(BlueprintCallable, Category = "Mouse Manager")
    void SetMouseVisibility(APlayerController* PC, bool bShow);

private:
    UPROPERTY()
    UUserWidget* CurrentCursorWidget = nullptr;
	
};
