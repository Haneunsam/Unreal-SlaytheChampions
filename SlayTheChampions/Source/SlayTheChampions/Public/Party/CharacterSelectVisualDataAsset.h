#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterSelectVisualDataAsset.generated.h"

class UAnimInstance;
class UAnimationAsset;
class UMaterialInterface;
class USkeletalMesh;

USTRUCT(BlueprintType)
struct SLAYTHECHAMPIONS_API FCharacterSelectVisualData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Visual")
	FName UnitID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Visual")
	TObjectPtr<USkeletalMesh> SkeletalMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Visual")
	TSubclassOf<UAnimInstance> AnimClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Visual")
	TObjectPtr<UAnimationAsset> IdleAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Visual")
	bool bLoopIdleAnimation = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Visual")
	TArray<TObjectPtr<UMaterialInterface>> OverrideMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Visual")
	FTransform RelativeTransform = FTransform::Identity;
};

UCLASS(BlueprintType)
class SLAYTHECHAMPIONS_API UCharacterSelectVisualDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Character Visual")
	bool FindVisualData(FName UnitID, FCharacterSelectVisualData& OutVisualData) const;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Visual", meta = (AllowPrivateAccess = "true"))
	TArray<FCharacterSelectVisualData> Visuals;
};
