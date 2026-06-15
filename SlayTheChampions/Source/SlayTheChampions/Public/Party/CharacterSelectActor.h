#pragma once

#include "CoreMinimal.h"
#include "Card/CardDataTypes.h"
#include "GameFramework/Actor.h"
#include "CharacterSelectActor.generated.h"

class UBoxComponent;
class UCharacterSelectVisualDataAsset;
class USkeletalMeshComponent;
class UTexture2D;

USTRUCT(BlueprintType)
struct FSelectableCharacterInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	FName UnitID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character", meta = (MultiLine = "true"))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	EJobClass JobClass = EJobClass::Warrior;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<UTexture2D> Portrait = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterSelected, ACharacterSelectActor*, SelectActor, const FSelectableCharacterInfo&, CharacterInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCharacterSelectionChanged, ACharacterSelectActor*, SelectActor, const FSelectableCharacterInfo&, CharacterInfo, bool, bInSelected);

UCLASS(BlueprintType, Blueprintable)
class SLAYTHECHAMPIONS_API ACharacterSelectActor : public AActor
{
	GENERATED_BODY()

public:
	ACharacterSelectActor();

	UFUNCTION(BlueprintCallable, Category = "CharacterSelect")
	void SetUnitID(FName InUnitID);

	UFUNCTION(BlueprintCallable, Category = "CharacterSelect")
	bool ApplyVisualDataByUnitID();

	UFUNCTION(BlueprintCallable, Category = "CharacterSelect")
	bool SelectCharacter();

	UFUNCTION(BlueprintCallable, Category = "CharacterSelect")
	bool DeselectCharacter();

	UFUNCTION(BlueprintCallable, Category = "CharacterSelect")
	bool ToggleCharacterSelection();

	UFUNCTION(BlueprintPure, Category = "CharacterSelect")
	const FSelectableCharacterInfo& GetCharacterInfo() const { return CharacterInfo; }

	UFUNCTION(BlueprintPure, Category = "CharacterSelect")
	bool IsSelected() const { return bSelected; }

	UPROPERTY(BlueprintAssignable, Category = "CharacterSelect")
	FOnCharacterSelected OnCharacterSelected;

	UPROPERTY(BlueprintAssignable, Category = "CharacterSelect")
	FOnCharacterSelectionChanged OnCharacterSelectionChanged;

protected:
	virtual void BeginPlay() override;
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;
	virtual void NotifyActorBeginCursorOver() override;
	virtual void NotifyActorEndCursorOver() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterSelect")
	void OnCharacterInfoLoaded(const FSelectableCharacterInfo& LoadedInfo);

	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterSelect")
	void OnSelectionStateChanged(bool bInSelected);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterSelect")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterSelect")
	TObjectPtr<USkeletalMeshComponent> PreviewMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterSelect")
	TObjectPtr<UBoxComponent> ClickBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterSelect")
	FSelectableCharacterInfo CharacterInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterSelect|Visual")
	TObjectPtr<UCharacterSelectVisualDataAsset> VisualDataAsset = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterSelect|Visual")
	bool bLoadDefaultVisualDataAsset = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterSelect")
	bool bDisableAfterSelected = false;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterSelect")
	bool bSelected = false;

private:
	void LoadUnitDataByID();
	void LoadDefaultVisualDataAsset();
	void SetHoverOutline(bool bEnabled);
};
