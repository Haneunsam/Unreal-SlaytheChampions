// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CombatTypes.h"
#include "Unit.generated.h"

class UStatComponent;

/**
 * 전투에 등장하는 모든 액터의 베이스 Pawn.
 * 자체적으로 어떤 컴포넌트도 들고 있지 않으며,
 * 블루프린트 또는 스폰 코드에서 필요한 컴포넌트를 부착해 능력을 부여한다.
 */


//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitDied, AUnit*, Unit);



UCLASS()
class SLAYTHECHAMPIONS_API AUnit : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AUnit();

	//유닛 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	FName UnitID;

	//유닛 소속
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	ETeam Team = ETeam::Enemy;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitDied, AUnit*, Unit);
	UPROPERTY(BlueprintAssignable, Category = "Unit")
	FOnUnitDied OnUnitDied;

	//
	UFUNCTION(BlueprintCallable, Category = "Unit")
	void HandleDeath();

	//StatComponent를 멤버로 두지 않고 Find로 꺼내기
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Unit")
	UStatComponent* GetStat() const;

	//StatComponent참조
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Unit")
	bool IsAlive() const;

	//StatComponent를 멤버로 안두는 이유는 AUnit은 무조건 StatComponent를 가지는 유닛으로만 굳어지는데,
	//덫이나 함정같이 HP가 필요없는 액터도 Unit을 베이스로 둘때 사용
	//FindComponentByClass<>로 꺼내면 컴포넌트가 없는 액터는 nullptr를 받아서 다르게 처리하면 됨


protected:
	
	virtual void BeginPlay() override;

};
