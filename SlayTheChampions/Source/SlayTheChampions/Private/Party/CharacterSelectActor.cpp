#include "Party/CharacterSelectActor.h"

#include "Components/BoxComponent.h"
#include "Components/MeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Party/PartyInstance.h"

ACharacterSelectActor::ACharacterSelectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	PreviewMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh"));
	PreviewMesh->SetupAttachment(SceneRoot);
	PreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ClickBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ClickBox"));
	ClickBox->SetupAttachment(SceneRoot);
	ClickBox->SetBoxExtent(FVector(80.f, 80.f, 120.f));
	ClickBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ClickBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	ClickBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void ACharacterSelectActor::BeginPlay()
{
	Super::BeginPlay();

	LoadUnitDataByID();
}

void ACharacterSelectActor::SetUnitID(FName InUnitID)
{
	CharacterInfo.UnitID = InUnitID;
	LoadUnitDataByID();
}

bool ACharacterSelectActor::SelectCharacter()
{
	if (bSelected)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CharacterSelectActor] SelectCharacter skipped. Already selected UnitID=%s"), *CharacterInfo.UnitID.ToString());
		return true;
	}

	if (CharacterInfo.UnitID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[CharacterSelectActor] SelectCharacter failed. UnitID is None. Actor=%s"), *GetName());
		return false;
	}

	UPartyInstance* PartyInstance = GetGameInstance() ? GetGameInstance()->GetSubsystem<UPartyInstance>() : nullptr;
	if (!PartyInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CharacterSelectActor] SelectCharacter failed. PartyInstance is null. Actor=%s"), *GetName());
		return false;
	}

	const bool bAdded = PartyInstance->AddPartyMember(CharacterInfo.UnitID, CharacterInfo.JobClass);
	if (!bAdded)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CharacterSelectActor] Character already selected or add failed. UnitID=%s"), *CharacterInfo.UnitID.ToString());
		return false;
	}

	bSelected = true;
	if (bDisableAfterSelected && ClickBox)
	{
		ClickBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	OnCharacterSelected.Broadcast(this, CharacterInfo);
	OnCharacterSelectionChanged.Broadcast(this, CharacterInfo, bSelected);
	OnSelectionStateChanged(bSelected);
	return true;
}

bool ACharacterSelectActor::DeselectCharacter()
{
	if (!bSelected)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CharacterSelectActor] DeselectCharacter skipped. Already deselected UnitID=%s"), *CharacterInfo.UnitID.ToString());
		return true;
	}

	if (CharacterInfo.UnitID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[CharacterSelectActor] DeselectCharacter failed. UnitID is None. Actor=%s"), *GetName());
		return false;
	}

	UPartyInstance* PartyInstance = GetGameInstance() ? GetGameInstance()->GetSubsystem<UPartyInstance>() : nullptr;
	if (!PartyInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CharacterSelectActor] DeselectCharacter failed. PartyInstance is null. Actor=%s"), *GetName());
		return false;
	}

	const bool bRemoved = PartyInstance->RemovePartyMember(CharacterInfo.UnitID, CharacterInfo.JobClass);
	if (!bRemoved)
	{
		return false;
	}

	bSelected = false;
	if (ClickBox)
	{
		ClickBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	OnCharacterSelectionChanged.Broadcast(this, CharacterInfo, bSelected);
	OnSelectionStateChanged(bSelected);
	return true;
}

bool ACharacterSelectActor::ToggleCharacterSelection()
{
	return bSelected ? DeselectCharacter() : SelectCharacter();
}

void ACharacterSelectActor::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	ToggleCharacterSelection();
}

void ACharacterSelectActor::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();
	SetHoverOutline(true);
}

void ACharacterSelectActor::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();
	SetHoverOutline(false);
}

void ACharacterSelectActor::LoadUnitDataByID()
{
	if (CharacterInfo.UnitID.IsNone())
	{
		return;
	}

	if (CharacterInfo.DisplayName.IsEmpty())
	{
		CharacterInfo.DisplayName = FText::FromName(CharacterInfo.UnitID);
	}

	OnCharacterInfoLoaded(CharacterInfo);
}

void ACharacterSelectActor::SetHoverOutline(bool bEnabled)
{
	TArray<UMeshComponent*> Meshes;
	GetComponents<UMeshComponent>(Meshes);
	for (UMeshComponent* Mesh : Meshes)
	{
		if (!Mesh)
		{
			continue;
		}

		Mesh->SetRenderCustomDepth(bEnabled);
		if (bEnabled)
		{
			Mesh->SetCustomDepthStencilValue(1);
		}
	}
}
