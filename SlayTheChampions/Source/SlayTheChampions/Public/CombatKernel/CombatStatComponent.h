#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatStatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCombatHPChanged, int32, Current, int32, Max);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FOnDefenceChanged, int32, Current);

UCLASS(ClassGroup = "Combat", meta = (BlueprintSpawnableComponent))
class SLAYTHECHAMPIONS_API UCombatStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatStatComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 MaxHP = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 CurrentHP = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Defence = 0;

	UPROPERTY(BlueprintAssignable, Category = "Stats|Event")
	FOnCombatHPChanged OnHPChanged;

	UPROPERTY(BlueprintAssignable, Category = "Stats|Event")
	FOnDefenceChanged OnDefenceChanged;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void TakeDamage(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddDefence(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void Heal(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void ResetDefence();

	UFUNCTION(BlueprintPure, Category = "Stats")
	bool IsDead() const { return CurrentHP <= 0; }

protected:
	virtual void BeginPlay() override;
};
