// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Unit/Enemy/EnemyDefinitionData.h"
#include "EnemyInitializerComponent.generated.h"

class StatComponent;
class NPCBrainComponent;
class StatusEffectComponent;

UCLASS( ClassGroup=(Unit), meta=(BlueprintSpawnableComponent) )
class SLAYTHECHAMPIONS_API UEnemyInitializerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyInitializerComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	TObjectPtr<UEnemyDefinitionData> Definition;

	// 스폰 직후 1회 호출 유닛 구성
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void InitializeFromDefinition(UEnemyDefinitionData* Def);
	
		
};
