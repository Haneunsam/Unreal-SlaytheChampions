#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CombatStatWidget.generated.h"

class UTextBlock;
class UCombatStatComponent;

/**
 * 전투원 머리 위에 띄우는 스탯 위젯 (HP / 방어도).
 * CombatantActor의 WidgetComponent에 할당.
 * InitWidget()으로 StatComponent를 연결하면 이후 자동 갱신.
 */
UCLASS()
class SLAYTHECHAMPIONS_API UCombatStatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// StatComponent 연결 + 초기값 표시 + 델리게이트 바인딩
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void InitWidget(UCombatStatComponent* InStat);

protected:
	// BP에서 이름 맞춰 바인딩 (TextBlock 이름이 정확히 일치해야 함)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_HP;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Defence;

private:
	UPROPERTY()
	UCombatStatComponent* StatComp;

	UFUNCTION()
	void OnHPChanged(int32 Current, int32 Max);

	UFUNCTION()
	void OnDefenceChanged(int32 Current);
};
