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
#include "Map/MapEnum.h"
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

	// BP_Title에 배치한 자식 액터들을 자동으로 찾아 타이틀 흐름에 연결한다.
	if (bAutoRegisterChildCharacterActors)
	{
		AutoRegisterChildCharacterActors();
	}

	if (bAutoBindNamedTitleClickActors)
	{
		AutoBindNamedTitleClickActors();
	}

	BindClickChildActor(NewGameClickChildActor, NewGameClickComponent);
	BindClickChildActor(LoadGameClickChildActor, LoadGameClickComponent);
	BindClickChildActor(ExitGameClickChildActor, ExitGameClickComponent);
	BindClickChildActor(StartRunClickChildActor, StartRunClickComponent);
	BindClickComponent(NewGameClickChildActor ? nullptr : NewGameClickComponent);
	BindClickComponent(LoadGameClickChildActor ? nullptr : LoadGameClickComponent);
	BindClickComponent(ExitGameClickChildActor ? nullptr : ExitGameClickComponent);
	BindClickComponent(StartRunClickChildActor ? nullptr : StartRunClickComponent);

	for (ACharacterSelectActor* CharacterActor : CharacterSelectActors)
	{
		RegisterCharacterSelectActor(CharacterActor);
	}

	SpawnTitleCamera();

	// 서브레벨에서 RunMap으로 돌아왔을 때 포탈 카메라로 다시 맞추기 위한 이벤트다.
	if (ULevelManager* LevelManager = GetGameInstance() ? GetGameInstance()->GetSubsystem<ULevelManager>() : nullptr)
	{
		LevelManager->OnStreamedLevelEntered.AddUniqueDynamic(this, &ATitleFlowActor::HandleStreamedLevelEntered);
	}

	// 처음 MainMap을 열 때는 타이틀을 보여주고, 복귀 상황만 다음 틱에서 보정한다.
	FocusTitleMenu(0.f);
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(this, &ATitleFlowActor::CheckInitialMapCameraFocus);
	}

	if (bDisableCharacterSelectionOnTitle)
	{
		SetCharacterSelectionEnabled(false);
	}

	OnLoadButtonAvailabilityChanged.Broadcast(CanLoadSave());
	RefreshPartySelectionState();

	// 위젯 컴포넌트 내부 텍스트는 BeginPlay 직후 준비가 늦을 수 있어 약간 지연 갱신한다.
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
	// 새 게임은 세이브와 카드 저장을 지우고 파티 선택부터 다시 시작한다.
	if (USTCGameInstance* STCGameInstance = Cast<USTCGameInstance>(GetGameInstance()))
	{
		STCGameInstance->DeleteSaveGameData();
	}

	UGameplayStatics::DeleteGameInSlot(UCardSaveGame::SlotName, 0);

	if (URunSystem* RunSystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<URunSystem>() : nullptr)
	{
		RunSystem->ResetRunProgressForNewGame();
	}

	if (UPartyInstance* PartyInstance = GetGameInstance() ? GetGameInstance()->GetSubsystem<UPartyInstance>() : nullptr)
	{
		PartyInstance->InitParty();
		PartyInstance->ClearChampionJobs();
		PartyInstance->ClearChampions();
	}

	OnLoadButtonAvailabilityChanged.Broadcast(CanLoadSave());
	FocusCharacterSelect();
}

void ATitleFlowActor::HandleLoadGameClicked()
{
	if (!CanLoadSave())
	{
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
	// 타이틀에서 맵으로 직접 넘어가는 경우에는 RunMap 진입 이벤트에서도 맵 카메라를 허용한다.
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
	RefreshPartySelectionState();
}

void ATitleFlowActor::HandleCharacterSelectionChanged(ACharacterSelectActor* SelectActor, const FSelectableCharacterInfo& CharacterInfo, bool bInSelected)
{
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
	if (!bFocusMapCameraWhenMapLevelEntered || LevelName != MapLevelName)
	{
		return;
	}

	const ULevelManager* LevelManager = GetGameInstance() ? GetGameInstance()->GetSubsystem<ULevelManager>() : nullptr;
	const FName PreviousLevel = LevelManager ? LevelManager->GetLastPreviousStreamedLevelName() : NAME_None;
	if (!bAllowMapCameraFocusOnMapLevelEntered && PreviousLevel.IsNone())
	{
		return;
	}

	if (!ShouldFocusMapCameraForRunMap())
	{
		return;
	}

	// 처음 실행은 타이틀을 유지하고, 다른 서브레벨에서 RunMap으로 복귀한 경우에만 포탈 카메라로 맞춘다.
	FocusMapCameraOnly(DefaultBlendTime);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(this, &ATitleFlowActor::RetryFocusMapCameraAfterRunMapEntered);
	}
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
		return;
	}

	if (!ActiveCamera)
	{
		SpawnTitleCamera();
	}

	if (!ActiveCamera)
	{
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

bool ATitleFlowActor::ShouldFocusMapCameraForRunMap() const
{
	if (bAllowMapCameraFocusOnMapLevelEntered)
	{
		return true;
	}

	const ULevelManager* LevelManager = GetGameInstance() ? GetGameInstance()->GetSubsystem<ULevelManager>() : nullptr;
	if (LevelManager && LevelManager->GetCurrentStreamedLevelName() == MapLevelName && !LevelManager->GetLastPreviousStreamedLevelName().IsNone())
	{
		return true;
	}

	const URunSystem* RunSystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<URunSystem>() : nullptr;
	if (!RunSystem)
	{
		return false;
	}

	const ERunState RunState = RunSystem->GetRunState();
	const bool bShouldFocus =
		RunState == ERunState::StageSelect ||
		RunState == ERunState::StageClear ||
		RunState == ERunState::RoomEntered ||
		RunState == ERunState::RunClear ||
		RunState == ERunState::RunFail;

	return bShouldFocus;
}

void ATitleFlowActor::CheckInitialMapCameraFocus()
{
	const ULevelManager* LevelManager = GetGameInstance() ? GetGameInstance()->GetSubsystem<ULevelManager>() : nullptr;
	if (!LevelManager)
	{
		return;
	}

	const FName CurrentLevel = LevelManager->GetCurrentStreamedLevelName();
	const FName PreviousLevel = LevelManager->GetLastPreviousStreamedLevelName();

	if (CurrentLevel == MapLevelName && !PreviousLevel.IsNone())
	{
		HandleStreamedLevelEntered(CurrentLevel);
	}
}

void ATitleFlowActor::RetryFocusMapCameraAfterRunMapEntered()
{
	if (!bFocusMapCameraWhenMapLevelEntered || !ShouldFocusMapCameraForRunMap())
	{
		return;
	}

	FocusMapCameraOnly(0.f);
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
		}
	}
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
	// 자식 액터 컴포넌트 이름으로 타이틀 버튼용 클릭 액터를 자동 연결한다.
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
