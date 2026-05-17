#include "Map/DebugRunInputActor.h"

#include "GameFramework/PlayerController.h"
#include "GameManagers/LevelManager.h"
#include "InputCoreTypes.h"

ADebugRunInputActor::ADebugRunInputActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADebugRunInputActor::BeginPlay()
{
	Super::BeginPlay();

	//디버그용
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		EnableInput(PlayerController);
		if (InputComponent)
		{
			InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &ADebugRunInputActor::DebugReturnToMap);
		}
	}
}

void ADebugRunInputActor::DebugReturnToMap()
{
	//디버그용
	if (!GetWorld())
	{
		return;
	}

	const FString CurrentMapName = GetWorld()->RemovePIEPrefix(GetWorld()->GetMapName());
	if (CurrentMapName == ReturnLevelName.ToString())
	{
		return;
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (ULevelManager* LevelManager = GameInstance->GetSubsystem<ULevelManager>())
		{
			LevelManager->GoToLevel(ReturnLevelName);
		}
	}
}
