#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelManager.generated.h"

UCLASS()
class SLAYTHECHAMPIONS_API ULevelManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

private:
   /*이동할 레벨의 이름*/
    FName MoveLevelName;

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    /*
    다른 레벨로 이동
    @param LevelName 이동할 레벨의 이름
    */
    void GoToLevel(FName LevelName);

    /*비동기 레벨 로드 호출*/
    void AsynchronousLoadLevel();

    /*비동기 레벨 호출 완료시 시작될 이벤트*/
    UFUNCTION()
    void OnLevelLoadCompleted();

    /*Level열릴시 호출되는 이벤트*/
    UFUNCTION()
    void OnLevelStarted(UWorld* World);
};
