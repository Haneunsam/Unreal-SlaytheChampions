#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CombatKernel/CardInstance.h"
#include "CardPlayerController.generated.h"

UCLASS()
class SLAYTHECHAMPIONS_API ACardPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    UFUNCTION(Client, Reliable, BlueprintCallable)
    void API_MoveCardToPositionAnimated(UCanvasPanel* Target, float PositionX, float PositionY);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<ACardInstance> CardInstance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator SpawnRotation;
};