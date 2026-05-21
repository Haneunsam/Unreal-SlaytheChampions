// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/Enemy/IntentComponent.h"

// Sets default values for this component's properties
UIntentComponent::UIntentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UIntentComponent::SetIntent(const FIntent& InIntent)
{
	Current = InIntent;
	OnIntentChanged.Broadcast(Current);
}

void UIntentComponent::ClearIntent()
{
	Current = FIntent();
	OnIntentChanged.Broadcast(Current);
}



