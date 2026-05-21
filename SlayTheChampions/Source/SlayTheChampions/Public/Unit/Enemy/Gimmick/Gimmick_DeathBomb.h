// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit/Enemy/GimmickComponent.h"
#include "Gimmick_DeathBomb.generated.h"

/**
 * 받은 피해량을 저장해두었다가 죽었을 때 상대에게 주기
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBombStackChanged, int32, OldStacks, int32, NewStacks);

UCLASS(ClassGroup = (Unit),meta = (BlueprintSpawnableComponent))
class SLAYTHECHAMPIONS_API UGimmick_DeathBomb : public UGimmickComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Gimmick|Bomb")
	int32 BombDamage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Bomb")
	FText DeathAnnounce;

protected:
	virtual void HandleHPChanged(int32 OldHP, int32 NewHP) override;
	
	virtual void HandleOwnerDied(AUnit* Unit) override;
};
