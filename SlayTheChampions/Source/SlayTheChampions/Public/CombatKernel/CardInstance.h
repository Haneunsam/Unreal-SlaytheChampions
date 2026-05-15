// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TimelineComponent.h"
#include "CardInstance.generated.h"


UCLASS()
class SLAYTHECHAMPIONS_API ACardInstance : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACardInstance();

	UPROPERTY(EditInstanceOnly, Category = "Animataion")
	UCurveFloat* MoveCurve;

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void API_MoveCardToPositionAnimated(UCanvasPanel* Target, float PositionX, float PositionY);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FTimeline MoveTimeline;

	UCanvasPanel* TargetPanel;
	float TargetX;
	float TargetY;
	float StartX;   // 시작 위치 저장 (Lerp A)
	float StartY;

	UFUNCTION()
	void OnTimelineUpdate(float Value);

	UFUNCTION()
	void OnTimelineFinished();

};
