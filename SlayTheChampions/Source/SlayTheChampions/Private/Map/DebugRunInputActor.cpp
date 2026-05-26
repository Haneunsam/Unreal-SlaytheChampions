#include "Map/DebugRunInputActor.h"

#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Map/RunSystem.h"

ADebugRunInputActor::ADebugRunInputActor()
{
	PrimaryActorTick.bCanEverTick = false;
	AutoReceiveInput = EAutoReceiveInput::Player0;
}

void ADebugRunInputActor::BeginPlay()
{
	Super::BeginPlay();

	//?붾쾭洹몄슜
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
	//?붾쾭洹몄슜
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
		//?붾쾭洹몄슜
		if (URunSystem* RunSystem = GameInstance->GetSubsystem<URunSystem>())
		{
			RunSystem->AreaCleared();
		}
	}
}