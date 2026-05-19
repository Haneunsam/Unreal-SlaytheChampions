#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatantActor.generated.h"

class UCombatStatComponent;
class UWidgetComponent;
class UCombatStatWidget;

/**
 * 전투 참가자(플레이어/적) 공통 베이스 액터.
 * CombatManager가 에디터에서 직접 연결한 뒤 FCombatantInitData를 주입함.
 * 비주얼(메시, 애니메이션)은 BP에서 추가.
 */
UCLASS()
class SLAYTHECHAMPIONS_API ACombatantActor : public AActor
{
	GENERATED_BODY()

public:
	ACombatantActor();

	// 스탯 데이터 + 이벤트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UCombatStatComponent* StatComponent;

	// 비주얼 메시 (BP에서 에셋 지정)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	UStaticMeshComponent* Mesh;

	// 머리 위 스탯 UI (Screen Space — 거리 무관 고정 크기)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* StatWidgetComponent;

	// BP에서 WBP_CombatantStat 지정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UCombatStatWidget> StatWidgetClass;

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
};
