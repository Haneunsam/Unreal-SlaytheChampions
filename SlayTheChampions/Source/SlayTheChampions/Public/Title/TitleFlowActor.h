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
	TitleMenu,
	CharacterSelect,
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Title|Camera")
	TObjectPtr<UArrowComponent> CharacterSelectCameraSlot;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Title|Camera")
	TObjectPtr<USceneComponent> MapCameraSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Title|Camera")
	TSubclassOf<ACameraActor> CameraClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Title|Camera", meta = (ClampMin = "0.0"))
	float DefaultBlendTime = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Title")
	FName MapLevelName = TEXT("RunMap");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Title")
	bool bFocusMapCameraWhenMapLevelEntered = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Title", meta = (ClampMin = "1"))
	int32 MinPartyMembersToStart = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Title", meta = (ClampMin = "0.0"))
	float InitialTextRefreshDelay = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title|Click")
	TObjectPtr<UPrimitiveComponent> NewGameClickComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title|Click")
	TObjectPtr<UPrimitiveComponent> LoadGameClickComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title|Click")
	TObjectPtr<UPrimitiveComponent> ExitGameClickComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title|Click")
	TObjectPtr<UPrimitiveComponent> StartRunClickComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title|Click")
	TObjectPtr<UChildActorComponent> NewGameClickChildActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title|Click")
	TObjectPtr<UChildActorComponent> LoadGameClickChildActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title|Click")
	TObjectPtr<UChildActorComponent> ExitGameClickChildActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title|Click")
	TObjectPtr<UChildActorComponent> StartRunClickChildActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Title|Click")
	bool bAutoBindNamedTitleClickActors = true;

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
	float ResolveBlendTime(float OverrideBlendTime) const;
	void AutoRegisterChildCharacterActors();
	void SetStartRunClickEnabled(bool bEnabled);
	void BindClickComponent(UPrimitiveComponent* ClickComponent);
	void BindClickChildActor(UChildActorComponent* ChildActorComponent, TObjectPtr<UPrimitiveComponent>& BoundComponent);
	UPrimitiveComponent* FindClickablePrimitiveComponent(AActor* Actor) const;
	void AutoBindNamedTitleClickActors();
	UChildActorComponent* FindChildActorComponentByName(FName ComponentName) const;
};
