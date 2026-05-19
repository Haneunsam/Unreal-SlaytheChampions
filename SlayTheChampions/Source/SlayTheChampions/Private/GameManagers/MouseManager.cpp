// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManagers/MouseManager.h"
#include "GameFramework/PlayerController.h"

void UMouseManager::SetupMouseCursor(APlayerController* PC, TSubclassOf<UUserWidget> CursorWidgetClass)
{
    if (!PC || !CursorWidgetClass) return;

    if (CurrentCursorWidget)
    {
        CurrentCursorWidget->RemoveFromParent();
        CurrentCursorWidget = nullptr;
    }

    CurrentCursorWidget = CreateWidget<UUserWidget>(PC, CursorWidgetClass);
    if (CurrentCursorWidget)
    {
        PC->SetMouseCursorWidget(EMouseCursor::Default, CurrentCursorWidget);
    }

    SetMouseVisibility(PC, true);
}

void UMouseManager::SetMouseVisibility(APlayerController* PC, bool bShow)
{
    if (!PC) return;

    PC->bShowMouseCursor = bShow;

    if (bShow)
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
    }
    else
    {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
    }
}