#include "Map/DebugRunInputActor.h"

#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Map/RunSystem.h"

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
		//디버그용
		if (URunSystem* RunSystem = GameInstance->GetSubsystem<URunSystem>())
		{
			RunSystem->AreaCleared();
		}
	}
}
