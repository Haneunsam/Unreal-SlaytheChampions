#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DebugRunInputActor.generated.h"

UCLASS()
class SLAYTHECHAMPIONS_API ADebugRunInputActor : public AActor
{
	GENERATED_BODY()

public:
	ADebugRunInputActor();

protected:
	virtual void BeginPlay() override;

private:
	/*디버그용*/
	UFUNCTION()
	void DebugReturnToMap();

private:
	/*디버그용*/
	UPROPERTY(EditAnywhere, Category = "Debug")
	FName ReturnLevelName = TEXT("GridMap");
};
