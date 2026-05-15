// Fill out your copyright notice in the Description page of Project Settings.


#include "CardInstance.h"
#include "Components/CanvasPanelSlot.h"


// Sets default values
ACardInstance::ACardInstance()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACardInstance::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACardInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACardInstance::API_MoveCardToPositionAnimated_Implementation(UCanvasPanel* Target, float PositionX, float PositionY)
{
	TargetPanel = Target;
	TargetX = PositionX;
	TargetY = PositionY;

	if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Target->Slot))
	{
		FVector2D CurrentPos = Slot->GetPosition();
		StartX = CurrentPos.X;
		StartY = CurrentPos.Y;
	}

	MoveTimeline.PlayFromStart();
}


void ACardInstance::OnTimelineUpdate(float Value)
{
	if (!TargetPanel) return;

	UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(TargetPanel->Slot);
	if (!Slot) return;
	float NewX = FMath::Lerp(StartX, TargetX, Value);
	float NewY = FMath::Lerp(StartY, TargetY, Value);
	Slot->SetPosition(FVector2D(NewX, NewY));
}

void ACardInstance::OnTimelineFinished()
{
	Destroy();
}

