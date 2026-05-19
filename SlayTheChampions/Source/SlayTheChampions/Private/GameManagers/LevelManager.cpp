#include "GameManagers/LevelManager.h"

#include "Kismet/GameplayStatics.h"
#include "Map/DebugRunInputActor.h"
#include "Map/MapManager.h"
#include "Map/RunSystem.h"

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

		if (MapName == "TestLoading")
		{
			AsynchronousLoadLevel();
			return;
		}

		//디버그용
		SpawnDebugInputActor(World);

		//디버그용
		if (MapName == "GridMap")
		{
			if (UMapManager* MapManager = GetGameInstance()->GetSubsystem<UMapManager>())
			{
				if (MapManager->HasMapData())
				{
					MapManager->RestoreMapWorld();
				}
			}

			if (URunSystem* RunSystem = GetGameInstance()->GetSubsystem<URunSystem>())
			{
				RunSystem->RefreshRunState();
			}
		}
	}
}

void ULevelManager::OnLevelLoadCompleted()
{
	UGameplayStatics::OpenLevel(GetWorld(), MoveLevelName);
}

void ULevelManager::SpawnDebugInputActor(UWorld* World)
{
	//디버그용
	if (!World)
	{
		return;
	}

	World->SpawnActor<ADebugRunInputActor>();
}
