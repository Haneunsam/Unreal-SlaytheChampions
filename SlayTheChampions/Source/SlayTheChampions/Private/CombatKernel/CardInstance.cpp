// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatKernel/CardInstance.h"
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

	MoveTimeline.TickTimeline(DeltaTime);
}

void ACardInstance::API_MoveCardToPositionAnimated_Implementation(UCanvasPanel* Target, float PositionX, float PositionY)
{
	TargetPanel = Target;
	TargetX = PositionX;
	TargetY = PositionY;

	if (UCanvasPanelSlot* TargetSlot = Cast<UCanvasPanelSlot>(Target->Slot))
	{
		FVector2D CurrentPos = TargetSlot->GetPosition();
		StartX = CurrentPos.X;
		StartY = CurrentPos.Y;
	}

	if (MoveCurve)
	{
		FOnTimelineFloat UpdateCallback;
		FOnTimelineEvent FinishCallback;
		UpdateCallback.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(ACardInstance, OnTimelineUpdate));
		FinishCallback.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(ACardInstance, OnTimelineFinished));
		MoveTimeline.AddInterpFloat(MoveCurve, UpdateCallback);
		MoveTimeline.SetTimelineFinishedFunc(FinishCallback);
	}

	MoveTimeline.PlayFromStart();
}


void ACardInstance::OnTimelineUpdate(float Value)
{
	if (!TargetPanel) return;

	UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(TargetPanel->Slot);
	if (!PanelSlot) return;
	float NewX = FMath::Lerp(StartX, TargetX, Value);
	float NewY = FMath::Lerp(StartY, TargetY, Value);
	PanelSlot->SetPosition(FVector2D(NewX, NewY));
}

void ACardInstance::OnTimelineFinished()
{
	Destroy();
}

