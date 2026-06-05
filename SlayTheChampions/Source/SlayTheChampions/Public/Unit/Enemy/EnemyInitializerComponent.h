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
	UEnemyInitializerComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	TObjectPtr<UEnemyDefinitionData> Definition;

	// 스폰 직후 한번 실행
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void InitializeFromDefinition(UEnemyDefinitionData* Def);
	
		
};
