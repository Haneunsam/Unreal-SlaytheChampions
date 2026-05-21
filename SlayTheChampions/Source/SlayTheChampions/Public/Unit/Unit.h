// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CombatTypes.h"
#include "Unit.generated.h"

class UStatComponent;

/**
 * 전투에 참여하는 모든 유닛의 베이스 Pawn.
 * 유닛에는 어떤 컴포넌트도 강제 포함하지 않으며,
 * 서브클래스 또는 외부 코드에서 필요한 컴포넌트를 붙일 능력을 부여한다.
 */


//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitDied, AUnit*, Unit);



UCLASS()
class SLAYTHECHAMPIONS_API AUnit : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AUnit();

	// 유닛 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	FName UnitID;

	// 유닛 소속
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	ETeam Team = ETeam::Enemy;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitDied, AUnit*, Unit);
	UPROPERTY(BlueprintAssignable, Category = "Unit")
	FOnUnitDied OnUnitDied;

	//
	UFUNCTION(BlueprintCallable, Category = "Unit")
	void HandleDeath();

	// StatComponent를 직접 저장하지 않고 Find로 가져옴
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Unit")
	UStatComponent* GetStat() const;

	// StatComponent에서 생존 여부 반환
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Unit")
	bool IsAlive() const;

	// StatComponent를 직접 저장하면 안 되는 이유: AUnit을 상속한 StatComponent를 가진다고 가정하는데,
	// 나이나 서브클래스에 HP가 필요없는 유닛도 Unit을 베이스로 쓸 경우
	// FindComponentByClass<>로 없으면 컴포넌트가 없는 경우 nullptr을 받아서 다르게 처리하면 됨


protected:

	virtual void BeginPlay() override;

};
