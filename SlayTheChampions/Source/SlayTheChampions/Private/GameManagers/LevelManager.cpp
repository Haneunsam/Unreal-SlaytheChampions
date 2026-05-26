#include "GameManagers/LevelManager.h"

#include "Kismet/GameplayStatics.h"

void ULevelManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ULevelManager::OnLevelStarted);
}

void ULevelManager::GoToLevel(FName LevelName)
{
    MoveLevelName = LevelName;

    UGameplayStatics::OpenLevel(GetWorld(), TEXT("TestLoading"));
}

void ULevelManager::AsynchronousLoadLevel()
{
    if (MoveLevelName.IsNone())
    {
        return;
    }

    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.ExecutionFunction = GET_FUNCTION_NAME_CHECKED(ULevelManager, OnLevelLoadCompleted);
    LatentInfo.Linkage = 0;
    LatentInfo.UUID = __LINE__;

    UGameplayStatics::LoadStreamLevel(GetWorld(), MoveLevelName, true, false, LatentInfo);
}

void ULevelManager::OnLevelStarted(UWorld* World)
{
    if (World)
    {
        FString MapName = World->RemovePIEPrefix(World->GetMapName());

        UE_LOG(LogTemp, Warning, TEXT("%s 이동 완료!"), *World->GetMapName());

        if (MapName == "TestLoading")
        {
            AsynchronousLoadLevel();
        }
    }
}

void ULevelManager::OnLevelLoadCompleted()
{
    UGameplayStatics::OpenLevel(GetWorld(), MoveLevelName);
}
