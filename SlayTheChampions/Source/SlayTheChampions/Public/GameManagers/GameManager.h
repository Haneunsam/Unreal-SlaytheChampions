// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameManager.generated.h"

UENUM(BlueprintType)
enum class EGameState : uint8
{
	Run = 0     UMETA(DisplayName = "진행"),
	Pause       UMETA(DisplayName = "정지"),
	Transition  UMETA(DisplayName = "씬 전환중"),
	Exit		UMETA(DisplayName = "게임 종료")
};

UCLASS()
class SLAYTHECHAMPIONS_API UGameManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
private:
	/*
	현재 게임 상태
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameManager", meta = (AllowPrivateAccess = "true", DisplayName = "현재 게임 상태"))
	EGameState CurrentState;

	class ULevelManager* LM;
public:

	/*SubSystem의 BeginPlay역할*/
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	/*게임 시작*/
	void StartGame();
	/*
	게임상태 변경
	@param state 변경할 상태(Run, Pause, Transition)
	*/
	void ChangeGameState(EGameState state);
	/*게임 종료*/
	void ExitGame();

	/*현재 게임상태 반환*/
	EGameState const GetGameState() const { return CurrentState; }
};
