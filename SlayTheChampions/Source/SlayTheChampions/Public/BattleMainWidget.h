// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class SLAYTHECHAMPIONS_API UBattleMainWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	// 위젯 초기화 시 호출
	virtual void NativeConstruct() override;

	// 블루프린트에서 접근 가능한 변수
	UPROPERTY(meta = (BindWidget))
	class UCanvas* MainCanvas;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button;

	

};
