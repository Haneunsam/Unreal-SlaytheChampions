#include "Title/TitleFlowActor.h"

#include "Camera/CameraActor.h"
#include "Card/CardSaveGame.h"
#include "Components/ArrowComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameManagers/LevelManager.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Map/RunSystem.h"
#include "Party/CharacterSelectActor.h"
#include "Party/PartyInstance.h"
#include "Save/STCGameInstance.h"

ATitleFlowActor::ATitleFlowActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	TitleCameraSlot = CreateDefaultSubobject<UArrowComponent>(TEXT("TitleCameraSlot"));
	TitleCameraSlot->SetupAttachment(SceneRoot);
	TitleCameraSlot->SetRelativeLocation(FVector(-500.f, 0.f, 200.f));
	TitleCameraSlot->ArrowColor = FColor::White;
	TitleCameraSlot->SetHiddenInGame(true);

	CharacterSelectCameraSlot = CreateDefaultSubobject<UArrowComponent>(TEXT("CharacterSelectCameraSlot"));
	CharacterSelectCameraSlot->SetupAttachment(SceneRoot);
	CharacterSelectCameraSlot->SetRelativeLocation(FVector(-500.f, 500.f, 200.f));
	CharacterSelectCameraSlot->ArrowColor = FColor::Green;
	CharacterSelectCameraSlot->SetHiddenInGame(true);
}

void ATitleFlowActor::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoRegisterChildCharacterActors)
	{
		AutoRegisterChildCharacterActors();
	}

	if (bAutoBindNamedTitleClickActors)
	{
		AutoBindNamedTitleClickActors();
	}

	BindClickComponent(NewGameClickComponent);
	BindClickComponent(LoadGameClickComponent);
	BindClickComponent(ExitGameClickComponent);
	BindClickComponent(StartRunClickComponent);
	BindClickChildActor(NewGameClickChildActor, NewGameClickComponent);
	BindClickChildActor(LoadGameClickChildActor, LoadGameClickComponent);
	BindClickChildActor(ExitGameClickChildActor, ExitGameClickComponent);
	BindClickChildActor(StartRunClickChildActor, StartRunClickComponent);

	for (ACharacterSelectActor* CharacterActor : CharacterSelectActors)
	{
		RegisterCharacterSelectActor(CharacterActor);
	}

	SpawnTitleCamera();

	if (ULevelManager* LevelManager = GetGameInstance() ? GetGameInstance()->GetSubsystem<ULevelManager>() : nullptr)
	{
		LevelManager->OnStreamedLevelEntered.AddUniqueDynamic(this, &ATitleFlowActor::HandleStreamedLevelEntered);
	}

	FocusTitleMenu(0.f);

	if (bDisableCharacterSelectionOnTitle)
	{
		SetCharacterSelectionEnabled(false);
	}

	OnLoadButtonAvailabilityChanged.Broadcast(CanLoadSave());
	RefreshPartySelectionState();

	if (InitialTextRefreshDelay <= 0.f)
	{
		RequestTitleTextRefresh();
	}
	else if (UWorld* World = GetWorld())
	{
		FTimerHandle TextRefreshTimerHandle;
		World->GetTimerManager().SetTimer(
			TextRefreshTimerHandle,
			this,
			&ATitleFlowActor::RequestTitleTextRefresh,
			InitialTextRefreshDelay,
			false);
	}
}

void ATitleFlowActor::HandleNewGameClicked()
{
	if (USTCGameInstance* STCGameInstance = Cast<USTCGameInstance>(GetGameInstance()))
	{
		STCGameInstance->DeleteSaveGameData();
	}

	UGameplayStatics::DeleteGameInSlot(UCardSaveGame::SlotName, 0);

	if (UPartyInstance* PartyInstance = GetGameInstance() ? GetGameInstance()->GetSubsystem<UPartyInstance>() : nullptr)
	{
		PartyInstance->InitParty();
		PartyInstance->ClearChampionJobs();
		PartyInstance->ClearChampions();
	}

	UE_LOG(LogTemp, Warning, TEXT("[TitleFlowActor] NewGame clicked. Save deleted and party reset."));
	OnLoadButtonAvailabilityChanged.Broadcast(CanLoadSave());
	FocusCharacterSelect();
}

void ATitleFlowActor::HandleLoadGameClicked()
{
	if (!CanLoadSave())
	{
		UE_LOG(LogTemp, Warning, TEXT("[TitleFlowActor] LoadGame failed. Save file does not exist."));
		OnLoadButtonAvailabilityChanged.Broadcast(false);
		return;
	}

	if (USTCGameInstance* STCGameInstance = Cast<USTCGameInstance>(GetGameInstance()))
	{
		STCGameInstance->LoadGameData();
	}

	if (URunSystem* RunSystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<URunSystem>() : nullptr)
	{
		RunSystem->RefreshRunState();
	}

	UE_LOG(LogTemp, Warning, TEXT("[TitleFlowActor] LoadGame clicked. Moving to map."));
	FocusMapStart();
}

void ATitleFlowActor::HandleExitGameClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr, EQuitPreference::Quit, false);
}

void ATitleFlowActor::HandleStartRunClicked()
{
	if (!CanStartRun())
	{
		UE_LOG(LogTemp, Warning, TEXT("[TitleFlowActor] StartRun failed. PartyMemberCount is less than %d."), MinPartyMembersToStart);
		RefreshPartySelectionState();
		return;
	}

	if (URunSystem* RunSystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<URunSystem>() : nullptr)
	{
		RunSystem->StartRunWithMap();
	}

	if (USTCGameInstance* STCGameInstance = Cast<USTCGameInstance>(GetGameInstance()))
	{
		STCGameInstance->SaveGameData();
	}

	UE_LOG(LogTemp, Warning, TEXT("[TitleFlowActor] StartRun clicked. Moving to map."));
	FocusMapStart();
}

void ATitleFlowActor::FocusTitleMenu(float OverrideBlendTime)
{
	SetCharacterSelectionEnabled(false);
	MoveCameraToSlot(TitleCameraSlot, ETitleFlowState::TitleMenu, OverrideBlendTime);
}

void ATitleFlowActor::FocusCharacterSelect(float OverrideBlendTime)
{
	SetCharacterSelectionEnabled(true);
	MoveCameraToSlot(CharacterSelectCameraSlot, ETitleFlowState::CharacterSelect, OverrideBlendTime);
}

void ATitleFlowActor::FocusMapStart(float OverrideBlendTime)
{
	bAllowMapCameraFocusOnMapLevelEntered = true;
	FocusMapCameraOnly(OverrideBlendTime);

	if (ULevelManager* LevelManager = GetGameInstance() ? GetGameInstance()->GetSubsystem<ULevelManager>() : nullptr)
	{
		LevelManager->MoveToConfiguredLevel(MapLevelName);
	}
}

void ATitleFlowActor::FocusMapCameraOnly(float OverrideBlendTime)
{
	SetCharacterSelectionEnabled(false);
	MoveCameraToSlot(MapCameraSlot ? MapCameraSlot.Get() : CharacterSelectCameraSlot.Get(), ETitleFlowState::MapStart, OverrideBlendTime);
}

void ATitleFlowActor::RegisterCharacterSelectActor(ACharacterSelectActor* CharacterActor)
{
	if (!CharacterActor)
	{
		return;
	}

	CharacterSelectActors.AddUnique(CharacterActor);
	CharacterActor->OnCharacterSelectionChanged.AddUniqueDynamic(this, &ATitleFlowActor::HandleCharacterSelectionChanged);
}

void ATitleFlowActor::SetCharacterSelectionEnabled(bool bEnabled)
{
	for (ACharacterSelectActor* CharacterActor : CharacterSelectActors)
	{
		if (IsValid(CharacterActor))
		{
			CharacterActor->SetActorEnableCollision(bEnabled);
		}
	}
}

void ATitleFlowActor::RefreshPartySelectionState()
{
	const bool bCanStart = CanStartRun();
	SetStartRunClickEnabled(bCanStart);
	OnStartButtonVisibilityChanged.Broadcast(bCanStart);
	UE_LOG(LogTemp, Warning, TEXT("[TitleFlowActor] RefreshPartySelectionState CanStart=%s"), bCanStart ? TEXT("true") : TEXT("false"));
}

bool ATitleFlowActor::CanStartRun() const
{
	const UPartyInstance* PartyInstance = GetGameInstance() ? GetGameInstance()->GetSubsystem<UPartyInstance>() : nullptr;
	return PartyInstance && PartyInstance->GetPartyMemberCount() >= MinPartyMembersToStart;
}

bool ATitleFlowActor::CanLoadSave() const
{
	const USTCGameInstance* STCGameInstance = Cast<USTCGameInstance>(GetGameInstance());
	return STCGameInstance && STCGameInstance->HasSaveGameData();
}

void ATitleFlowActor::HandleCharacterSelected(ACharacterSelectActor* SelectActor, const FSelectableCharacterInfo& CharacterInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("[TitleFlowActor] Character selected. Actor=%s UnitID=%s"),
		SelectActor ? *SelectActor->GetName() : TEXT("None"),
		*CharacterInfo.UnitID.ToString());

	RefreshPartySelectionState();
}

void ATitleFlowActor::HandleCharacterSelectionChanged(ACharacterSelectActor* SelectActor, const FSelectableCharacterInfo& CharacterInfo, bool bInSelected)
{
	UE_LOG(LogTemp, Warning, TEXT("[TitleFlowActor] Character selection changed. Actor=%s UnitID=%s Selected=%s"),
		SelectActor ? *SelectActor->GetName() : TEXT("None"),
		*CharacterInfo.UnitID.ToString(),
		bInSelected ? TEXT("true") : TEXT("false"));

	RefreshPartySelectionState();
}

void ATitleFlowActor::RequestTitleTextRefresh()
{
	OnTitleTextRefreshRequested();
}

AActor* ATitleFlowActor::GetTitleChildActorByName(FName ComponentName) const
{
	UChildActorComponent* ChildActorComponent = FindChildActorComponentByName(ComponentName);
	return ChildActorComponent ? ChildActorComponent->GetChildActor() : nullptr;
}

void ATitleFlowActor::HandleNewGameComponentClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	HandleNewGameClicked();
}

void ATitleFlowActor::HandleLoadGameComponentClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	HandleLoadGameClicked();
}

void ATitleFlowActor::HandleExitGameComponentClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	HandleExitGameClicked();
}

void ATitleFlowActor::HandleStartRunComponentClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	HandleStartRunClicked();
}

void ATitleFlowActor::HandleStreamedLevelEntered(FName LevelName)
{
	if (!bFocusMapCameraWhenMapLevelEntered || !bAllowMapCameraFocusOnMapLevelEntered || LevelName != MapLevelName)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[TitleFlowActor] Map level entered. Focus map camera. Level=%s"), *LevelName.ToString());
	FocusMapCameraOnly(DefaultBlendTime);
}

void ATitleFlowActor::SpawnTitleCamera()
{
	if (ActiveCamera || !CameraClass || !TitleCameraSlot)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ActiveCamera = World->SpawnActor<ACameraActor>(CameraClass, TitleCameraSlot->GetComponentTransform(), SpawnParams);
	if (ActiveCamera)
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			PlayerController->SetViewTargetWithBlend(ActiveCamera, 0.f);
		}
	}
}

void ATitleFlowActor::MoveCameraToSlot(USceneComponent* CameraSlot, ETitleFlowState TargetState, float OverrideBlendTime)
{
	if (!CameraSlot)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TitleFlowActor] MoveCameraToSlot failed. CameraSlot is null."));
		return;
	}

	if (!ActiveCamera)
	{
		SpawnTitleCamera();
	}

	if (!ActiveCamera)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TitleFlowActor] MoveCameraToSlot failed. ActiveCamera is null."));
		return;
	}

	const FTransform TargetTransform = CameraSlot->GetComponentTransform();
	ActiveCamera->SetActorTransform(TargetTransform);

	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			PlayerController->SetViewTargetWithBlend(ActiveCamera, ResolveBlendTime(OverrideBlendTime));
		}
	}

	SetFlowState(TargetState);
	OnTitleCameraMoveRequested.Broadcast(TargetState, TargetTransform, ActiveCamera);
}

void ATitleFlowActor::SetFlowState(ETitleFlowState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	CurrentState = NewState;
	OnTitleFlowStateChanged.Broadcast(CurrentState);
}

float ATitleFlowActor::ResolveBlendTime(float OverrideBlendTime) const
{
	return OverrideBlendTime >= 0.f ? OverrideBlendTime : DefaultBlendTime;
}

void ATitleFlowActor::AutoRegisterChildCharacterActors()
{
	CharacterSelectActors.Reset();

	TArray<UChildActorComponent*> ChildActorComponents;
	GetComponents<UChildActorComponent>(ChildActorComponents);

	for (UChildActorComponent* ChildActorComponent : ChildActorComponents)
	{
		if (!ChildActorComponent)
		{
			continue;
		}

		if (ACharacterSelectActor* CharacterActor = Cast<ACharacterSelectActor>(ChildActorComponent->GetChildActor()))
		{
			RegisterCharacterSelectActor(CharacterActor);
			UE_LOG(LogTemp, Warning, TEXT("[TitleFlowActor] Registered child character. Component=%s Actor=%s UnitID=%s"),
				*ChildActorComponent->GetName(),
				*CharacterActor->GetName(),
				*CharacterActor->GetCharacterInfo().UnitID.ToString());
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[TitleFlowActor] AutoRegisterChildCharacterActors Count=%d"), CharacterSelectActors.Num());
}

void ATitleFlowActor::SetStartRunClickEnabled(bool bEnabled)
{
	if (!StartRunClickComponent)
	{
		return;
	}

	StartRunClickComponent->SetCollisionEnabled(bEnabled ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void ATitleFlowActor::BindClickComponent(UPrimitiveComponent* ClickComponent)
{
	if (!ClickComponent)
	{
		return;
	}

	ClickComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ClickComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	ClickComponent->SetGenerateOverlapEvents(false);

	if (ClickComponent == NewGameClickComponent)
	{
		ClickComponent->OnClicked.AddUniqueDynamic(this, &ATitleFlowActor::HandleNewGameComponentClicked);
	}
	else if (ClickComponent == LoadGameClickComponent)
	{
		ClickComponent->OnClicked.AddUniqueDynamic(this, &ATitleFlowActor::HandleLoadGameComponentClicked);
	}
	else if (ClickComponent == ExitGameClickComponent)
	{
		ClickComponent->OnClicked.AddUniqueDynamic(this, &ATitleFlowActor::HandleExitGameComponentClicked);
	}
	else if (ClickComponent == StartRunClickComponent)
	{
		ClickComponent->OnClicked.AddUniqueDynamic(this, &ATitleFlowActor::HandleStartRunComponentClicked);
	}

	UE_LOG(LogTemp, Warning, TEXT("[TitleFlowActor] BindClickComponent Component=%s"), *ClickComponent->GetName());
}

void ATitleFlowActor::BindClickChildActor(UChildActorComponent* ChildActorComponent, TObjectPtr<UPrimitiveComponent>& BoundComponent)
{
	if (!ChildActorComponent)
	{
		return;
	}

	AActor* ChildActor = ChildActorComponent->GetChildActor();
	UPrimitiveComponent* ClickComponent = FindClickablePrimitiveComponent(ChildActor);
	if (!ClickComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TitleFlowActor] BindClickChildActor failed. ChildActor=%s has no clickable primitive."),
			ChildActor ? *ChildActor->GetName() : TEXT("None"));
		return;
	}

	BoundComponent = ClickComponent;
	BindClickComponent(BoundComponent);
}

UPrimitiveComponent* ATitleFlowActor::FindClickablePrimitiveComponent(AActor* Actor) const
{
	if (!Actor)
	{
		return nullptr;
	}

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	Actor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

	for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
	{
		if (!PrimitiveComponent)
		{
			continue;
		}

		if (PrimitiveComponent->GetCollisionEnabled() != ECollisionEnabled::NoCollision)
		{
			return PrimitiveComponent;
		}
	}

	return PrimitiveComponents.IsEmpty() ? nullptr : PrimitiveComponents[0];
}

void ATitleFlowActor::AutoBindNamedTitleClickActors()
{
	if (!NewGameClickChildActor)
	{
		NewGameClickChildActor = FindChildActorComponentByName(TEXT("NewGame"));
	}

	if (!LoadGameClickChildActor)
	{
		LoadGameClickChildActor = FindChildActorComponentByName(TEXT("LoadGame"));
	}

	if (!ExitGameClickChildActor)
	{
		ExitGameClickChildActor = FindChildActorComponentByName(TEXT("Exit"));
	}

	if (!StartRunClickChildActor)
	{
		StartRunClickChildActor = FindChildActorComponentByName(TEXT("StartRun"));
		if (!StartRunClickChildActor)
		{
			StartRunClickChildActor = FindChildActorComponentByName(TEXT("StartGame"));
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[TitleFlowActor] AutoBindNamedTitleClickActors NewGame=%s LoadGame=%s Exit=%s Start=%s"),
		NewGameClickChildActor ? *NewGameClickChildActor->GetName() : TEXT("None"),
		LoadGameClickChildActor ? *LoadGameClickChildActor->GetName() : TEXT("None"),
		ExitGameClickChildActor ? *ExitGameClickChildActor->GetName() : TEXT("None"),
		StartRunClickChildActor ? *StartRunClickChildActor->GetName() : TEXT("None"));

	auto LogChildActorClass = [](const TCHAR* Label, UChildActorComponent* ChildActorComponent)
	{
		AActor* ChildActor = ChildActorComponent ? ChildActorComponent->GetChildActor() : nullptr;
		UE_LOG(LogTemp, Warning, TEXT("[TitleFlowActor] %s ChildActor=%s Class=%s ChildActorClass=%s"),
			Label,
			ChildActor ? *ChildActor->GetName() : TEXT("None"),
			ChildActor ? *ChildActor->GetClass()->GetName() : TEXT("None"),
			ChildActorComponent && ChildActorComponent->GetChildActorClass() ? *ChildActorComponent->GetChildActorClass()->GetName() : TEXT("None"));
	};

	LogChildActorClass(TEXT("NewGame"), NewGameClickChildActor);
	LogChildActorClass(TEXT("LoadGame"), LoadGameClickChildActor);
	LogChildActorClass(TEXT("Exit"), ExitGameClickChildActor);
	LogChildActorClass(TEXT("Start"), StartRunClickChildActor);
}

UChildActorComponent* ATitleFlowActor::FindChildActorComponentByName(FName ComponentName) const
{
	TArray<UChildActorComponent*> ChildActorComponents;
	GetComponents<UChildActorComponent>(ChildActorComponents);

	for (UChildActorComponent* ChildActorComponent : ChildActorComponents)
	{
		if (!ChildActorComponent)
		{
			continue;
		}

		if (ChildActorComponent->GetFName() == ComponentName)
		{
			return ChildActorComponent;
		}
	}

	return nullptr;
}
