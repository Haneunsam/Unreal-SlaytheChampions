// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatKernel/CardPlayerController.h"
#include "CombatKernel/CardInstance.h"
#include "Components/CanvasPanel.h"

void ACardPlayerController::API_MoveCardToPositionAnimated_Implementation(UCanvasPanel* Target, float PositionX, float PositionY)
{
    if (!CardInstance) return;

    FTransform SpawnTransform;

    ACardInstance* SpawnedCard = GetWorld()->SpawnActor<ACardInstance>(CardInstance, SpawnTransform);

    if (SpawnedCard)
    {
        // ✅ Target(CanvasPanel)을 그대로 넘김
        SpawnedCard->API_MoveCardToPositionAnimated(Target, PositionX, PositionY);
    }
}