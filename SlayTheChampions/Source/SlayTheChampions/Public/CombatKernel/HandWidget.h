#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Card/CardDataTypes.h"
#include "HandWidget.generated.h"

class UHorizontalBox;
class UCardWidget;

// 카드 클릭 시 브로드캐스트 — CardID를 BattleMainWidget에 전달
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardSelected, FName, CardID);

/**
 * UHandWidget
 * 플레이어 손패 패널의 C++ 베이스 클래스.
 * WBP_HandPanel이 이 클래스를 부모로 사용한다.
 *
 * 역할:
 *  - CardContainer(ScrollBox)에 카드 위젯 표시
 *  - ShowHand / ClearHand 공개 API 제공
 *
 * 버튼(EndTurn, NextPlayer)은 WBP_BattleMainWidget에서 직접 관리한다.
 */
UCLASS()
class SLAYTHECHAMPIONS_API UHandWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 카드 클릭 시 — WBP_HandPanel BP에서 Broadcast, BattleMainWidget이 구독
	// BlueprintCallable 추가: 소유 BP(WBP_HandPanel)에서도 Call(Broadcast) 가능
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Hand")
	FOnCardSelected OnCardSelected;

	// ── 공개 API ─────────────────────────────────────────────────────────
	// 손패 갱신: 기존 카드 제거 후 OnShowHand 브로드캐스트
	UFUNCTION(BlueprintCallable, Category = "Hand")
	void ShowHand(const TArray<FCardDataRow>& Cards);

	// 카드 컨테이너 비우기
	UFUNCTION(BlueprintCallable, Category = "Hand")
	void ClearHand();

	// ── 대기 카드 관리 ───────────────────────────────────────────────────
	// CardID와 일치하는 카드 위젯에 대기 상태 진입 — 호버 애니메이션 유지
	UFUNCTION(BlueprintCallable, Category = "Hand")
	void SetCardPending(FName CardID);

	// 위젯 포인터를 직접 받아 대기 상태 진입 — 동일 CardID가 여러 장일 때 정확한 위젯 지정
	UFUNCTION(BlueprintCallable, Category = "Hand")
	void SetCardPendingDirect(UCardWidget* Card);

	// 현재 대기 카드의 대기 상태 해제
	UFUNCTION(BlueprintCallable, Category = "Hand")
	void ClearCardPending();

	// ── BP 구현부 ─────────────────────────────────────────────────────────
	// WBP_HandPanel에서 카드 위젯(WBP_Card)을 생성하고 CardContainer에 추가
	UFUNCTION(BlueprintImplementableEvent, Category = "Hand")
	void OnShowHand(const TArray<FCardDataRow>& Cards);

	// 손패 숨김 역모션 재생 — BP에서 등장 역방향 애니메이션 구현 후 완료 시 ClearHand() + Hidden 처리
	UFUNCTION(BlueprintImplementableEvent, Category = "Hand")
	void PlayHideAnimation();

protected:
	// 카드 목록 가로 박스 — WBP에서 이름 'CardContainer' 일치 필요
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* CardContainer;

private:
	// 현재 대기 상태인 카드 위젯 — ClearCardPending 시 SetPendingState(false) 호출에 사용
	UPROPERTY()
	UCardWidget* PendingCard = nullptr;
};
