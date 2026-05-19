// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManagers/GameManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameManagers/LevelManager.h"

void UGameManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentState = EGameState::Run;
	LM = GetWorld()->GetGameInstance()->GetSubsystem<ULevelManager>();
}

void UGameManager::StartGame()
{
	//타이틀씬 or 시작씬에서 게임씬으로 넘어가는 로직 
	//LM -> GoToLevel(시작씬 이름)
}

void UGameManager::ChangeGameState(EGameState state)
{
    if (CurrentState == state) return;

	CurrentState = state;
}

void UGameManager::ExitGame()
{
	//추후 종료시 자동저장 필요하면 추가 코드 작성

	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}


