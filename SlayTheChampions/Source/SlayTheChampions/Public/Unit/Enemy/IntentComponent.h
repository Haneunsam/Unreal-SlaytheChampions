// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Unit/CombatTypes.h"
#include "IntentComponent.generated.h"

//행동을 저장과 알림만 하는 역할

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIntentChanged, const FIntent&, NewIntent);

UCLASS( ClassGroup=(Unit), meta=(BlueprintSpawnableComponent) )
class SLAYTHECHAMPIONS_API UIntentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIntentComponent();

	UPROPERTY(BlueprintReadOnly, Category = "Intent")
	FIntent Current;

	//NPCBrain이 다음 행동을 결정하면 호출
	UFUNCTION(BlueprintCallable, Category = "Intent")
	void SetIntent(const FIntent& InIntent);

	// 턴 종료 후 초기화
	UFUNCTION(BlueprintCallable, Category = "Intent")
	void ClearIntent();

	// UI 위젯은 이것만 구독하면 됨
	UPROPERTY(BlueprintAssignable, Category = "Intent")
	FOnIntentChanged OnIntentChanged;
		
};
