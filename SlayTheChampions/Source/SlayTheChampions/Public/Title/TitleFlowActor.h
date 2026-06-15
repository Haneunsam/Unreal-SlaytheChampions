#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TitleFlowActor.generated.h"

class ACameraActor;
class ACharacterSelectActor;
class UChildActorComponent;
class UPrimitiveComponent;
class UArrowComponent;

UENUM(BlueprintType)
enum class ETitleFlowState : uint8
{
	// 첫 화면: 새 게임, 불러오기, 종료 버튼을 보여주는 상태
	TitleMenu,
	// 새 게임 이후 파티원을 고르는 상태
	CharacterSelect,
	// 런 맵 포탈을 바라보는 상태
	MapStart
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTitleFlowStateChanged, ETitleFlowState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTitleStartButtonVisibilityChanged, bool, bShouldShow);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTitleLoadButtonAvailabilityChanged, bool, bCanLoad);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTitleCameraMoveRequested, ETitleFlowState, TargetState, const FTransform&, TargetTransform, ACameraActor*, CameraActor);

UCLASS(BlueprintType, Blueprintable)
class SLAYTHECHAMPIONS_API ATitleFlowActor : public AActor
{
	GENERATED_BODY()

public:
	ATitleFlowActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Title|Component")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Title|Camera")
	TObjectPtr<UArrowComponent> TitleCameraSlot;

	// 캐릭터 선택 화면을 바라볼 카메라 기준점
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Title|Camera")
	TObjectPtr<UArrowComponent> CharacterSelectCameraSlot;

	// RunMap 복귀 시 포탈 쪽을 바라볼 카메라 기준점. BP_LevelCameraSetter의 슬롯 등을 꽂아 쓴다.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Title|Camera")
	TObjectPtr<USceneComponent> MapCameraSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Title|Camera")
	TSubclassOf<ACameraActor> CameraClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Title|Camera", meta = (ClampMin = "0.0"))
	float DefaultBlendTime = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Title")
	FName MapLevelName = TEXT("RunMap");

	// 다른 서브레벨에서 RunMap으로 돌아왔을 때 자동으로 맵 카메라를 맞출지 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Title")
	bool bFocusMapCameraWhenMapLevelEntered = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Title", meta = (ClampMin = "1"))
	int32 MinPartyMembersToStart = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Title", meta = (ClampMin = "0.0"))
	float InitialTextRefreshDelay = 0.05f;

	// 직접 콜리전 컴포넌트를 지정하는 방식. 자식 액터를 쓰지 않을 때 사용한다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title|Click")
	TObjectPtr<UPrimitiveComponent> NewGameClickComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title|Click")
	TObjectPtr<UPrimitiveComponent> LoadGameClickComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title|Click")
	TObjectPtr<UPrimitiveComponent> ExitGameClickComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title|Click")
	TObjectPtr<UPrimitiveComponent> StartRunClickComponent;

	// BP_Title의 자식 액터 컴포넌트를 버튼으로 사용하는 방식. 내부의 첫 클릭 가능한 Primitive를 찾아 바인딩한다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title|Click")
	TObjectPtr<UChildActorComponent> NewGameClickChildActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title|Click")
	TObjectPtr<UChildActorComponent> LoadGameClickChildActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title|Click")
	TObjectPtr<UChildActorComponent> ExitGameClickChildActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title|Click")
	TObjectPtr<UChildActorComponent> StartRunClickChildActor;

	// true면 NewGame, LoadGame, Exit, StartRun 이름의 자식 액터 컴포넌트를 자동으로 찾는다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Title|Click")
	bool bAutoBindNamedTitleClickActors = true;

	// 캐릭터 선택 액터 목록. 자식 액터로 배치했다면 자동 등록을 켜두면 된다.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Title|Character")
	TArray<TObjectPtr<ACharacterSelectActor>> CharacterSelectActors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Title|Character")
	bool bAutoRegisterChildCharacterActors = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Title|Character")
	bool bDisableCharacterSelectionOnTitle = true;

	UPROPERTY(BlueprintReadOnly, Category = "Title|Runtime")
	TObjectPtr<ACameraActor> ActiveCamera;

	UPROPERTY(BlueprintReadOnly, Category = "Title|Runtime")
	ETitleFlowState CurrentState = ETitleFlowState::TitleMenu;

	// 타이틀 버튼으로 맵에 진입한 경우 RunMap 진입 이벤트에서 맵 카메라 보정을 허용한다.
	UPROPERTY(BlueprintReadOnly, Category = "Title|Runtime")
	bool bAllowMapCameraFocusOnMapLevelEntered = false;

public:
	UPROPERTY(BlueprintAssignable, Category = "Title")
	FOnTitleFlowStateChanged OnTitleFlowStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Title")
	FOnTitleStartButtonVisibilityChanged OnStartButtonVisibilityChanged;

	UPROPERTY(BlueprintAssignable, Category = "Title")
	FOnTitleLoadButtonAvailabilityChanged OnLoadButtonAvailabilityChanged;

	UPROPERTY(BlueprintAssignable, Category = "Title|Camera")
	FOnTitleCameraMoveRequested OnTitleCameraMoveRequested;

	UFUNCTION(BlueprintCallable, Category = "Title")
	void HandleNewGameClicked();

	UFUNCTION(BlueprintCallable, Category = "Title")
	void HandleLoadGameClicked();

	UFUNCTION(BlueprintCallable, Category = "Title")
	void HandleExitGameClicked();

	UFUNCTION(BlueprintCallable, Category = "Title")
	void HandleStartRunClicked();

	UFUNCTION(BlueprintCallable, Category = "Title")
	void FocusTitleMenu(float OverrideBlendTime = -1.f);

	UFUNCTION(BlueprintCallable, Category = "Title")
	void FocusCharacterSelect(float OverrideBlendTime = -1.f);

	UFUNCTION(BlueprintCallable, Category = "Title")
	void FocusMapStart(float OverrideBlendTime = -1.f);

	// 레벨 전환 없이 카메라만 맵 기준점으로 보정할 때 사용한다.
	UFUNCTION(BlueprintCallable, Category = "Title")
	void FocusMapCameraOnly(float OverrideBlendTime = -1.f);

	UFUNCTION(BlueprintCallable, Category = "Title|Character")
	void RegisterCharacterSelectActor(ACharacterSelectActor* CharacterActor);

	UFUNCTION(BlueprintCallable, Category = "Title|Character")
	void SetCharacterSelectionEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Title|Character")
	void RefreshPartySelectionState();

	UFUNCTION(BlueprintPure, Category = "Title")
	bool CanStartRun() const;

	UFUNCTION(BlueprintPure, Category = "Title")
	bool CanLoadSave() const;

	UFUNCTION(BlueprintPure, Category = "Title")
	ETitleFlowState GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintCallable, Category = "Title|Text")
	void RequestTitleTextRefresh();

	// WBP 텍스트 초기화가 늦는 경우 BP에서 이 이벤트를 받아 다시 텍스트를 세팅한다.
	UFUNCTION(BlueprintPure, Category = "Title|Text")
	AActor* GetTitleChildActorByName(FName ComponentName) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Title|Text")
	void OnTitleTextRefreshRequested();

private:
	UFUNCTION()
	void HandleCharacterSelected(ACharacterSelectActor* SelectActor, const struct FSelectableCharacterInfo& CharacterInfo);

	UFUNCTION()
	void HandleCharacterSelectionChanged(ACharacterSelectActor* SelectActor, const struct FSelectableCharacterInfo& CharacterInfo, bool bInSelected);

	UFUNCTION()
	void HandleNewGameComponentClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	UFUNCTION()
	void HandleLoadGameComponentClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	UFUNCTION()
	void HandleExitGameComponentClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	UFUNCTION()
	void HandleStartRunComponentClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	UFUNCTION()
	void HandleStreamedLevelEntered(FName LevelName);

	void SpawnTitleCamera();
	void MoveCameraToSlot(USceneComponent* CameraSlot, ETitleFlowState TargetState, float OverrideBlendTime);
	void SetFlowState(ETitleFlowState NewState);
	bool ShouldFocusMapCameraForRunMap() const;
	void CheckInitialMapCameraFocus();
	void RetryFocusMapCameraAfterRunMapEntered();
	float ResolveBlendTime(float OverrideBlendTime) const;
	void AutoRegisterChildCharacterActors();
	void SetStartRunClickEnabled(bool bEnabled);
	void BindClickComponent(UPrimitiveComponent* ClickComponent);
	void BindClickChildActor(UChildActorComponent* ChildActorComponent, TObjectPtr<UPrimitiveComponent>& BoundComponent);
	UPrimitiveComponent* FindClickablePrimitiveComponent(AActor* Actor) const;
	void AutoBindNamedTitleClickActors();
	UChildActorComponent* FindChildActorComponentByName(FName ComponentName) const;
};
