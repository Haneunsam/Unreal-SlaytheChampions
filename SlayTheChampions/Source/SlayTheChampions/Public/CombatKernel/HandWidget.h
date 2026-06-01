#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Card/CardDataTypes.h"
#include "HandWidget.generated.h"

class UCanvasPanel;
class UHorizontalBox;
class UTextBlock;
class UCardWidget;

// 카드 클릭 시 브로드캐스트 — CardID + 클릭된 위젯 포인터를 BattleMainWidget에 전달
// ClickedCard를 함께 전달해 동일 CardID가 여러 장일 때 정확한 위젯을 특정
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCardSelected, FName, CardID, UCardWidget*, ClickedCard);

/**
 * UHandWidget
 * 플레이어 손패 패널의 C++ 베이스 클래스.
 * WBP_HandPanel이 이 클래스를 부모로 사용한다.
 *
 * 레이아웃 규칙:
 *  - 5장 이하: 카드 사이 CardSpacing 간격으로 나란히 배치
 *  - 6장 이상: OverlapFraction(기본 1/10)씩 겹쳐 배치
 *  - 카드 크기는 CardWidth 기준 고정 (개수 무관)
 *  - 호버 시 해당 카드를 z-order 최상위로 올림
 */
UCLASS()
class SLAYTHECHAMPIONS_API UHandWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 카드 클릭 시 — WBP_HandPanel BP에서 Broadcast, BattleMainWidget이 구독
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Hand")
	FOnCardSelected OnCardSelected;

	// ── 레이아웃 설정 ────────────────────────────────────────────────────────
	// 카드 한 장 너비(px) — WBP_Card 실제 너비와 맞출 것
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand|Layout")
	float CardWidth = 150.f;

	// 5장 이하 일반 배치 시 카드 사이 간격(px)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand|Layout")
	float CardSpacing = 10.f;

	// 6장 이상 겹침 비율 (0.1 = 카드 너비의 1/10 겹침)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand|Layout", meta = (ClampMin = "0.0", ClampMax = "0.9"))
	float OverlapFraction = 0.1f;

	// 중앙 정렬 기준 너비 — CardCanvas 실제 크기를 못 읽을 때 폴백으로 사용 (FHD = 1920)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hand|Layout")
	float PanelWidth = 1920.f;

	// ── 공개 API ─────────────────────────────────────────────────────────
	// 손패 갱신: 기존 카드 제거 → OnShowHand → 레이아웃 적용
	UFUNCTION(BlueprintCallable, Category = "Hand")
	void ShowHand(const TArray<FCardDataRow>& Cards);

	// 뽑을 덱 / 버린 덱 카운트 텍스트 갱신
	UFUNCTION(BlueprintCallable, Category = "Hand")
	void UpdateDeckCounts(int32 DrawPile, int32 DiscardPile);

	// 카드 컨테이너 비우기
	UFUNCTION(BlueprintCallable, Category = "Hand")
	void ClearHand();

	// ── 대기 카드 관리 ───────────────────────────────────────────────────
	// 클릭된 위젯 포인터를 직접 받아 대기 상태로 — 동일 CardID 다중 보유 시 정확한 카드 특정
	UFUNCTION(BlueprintCallable, Category = "Hand")
	void SetCardPendingDirect(UCardWidget* Card);

	UFUNCTION(BlueprintCallable, Category = "Hand")
	void ClearCardPending();

	// ── z-order 관리 ─────────────────────────────────────────────────────
	// 타겟 지정 중 카드 인터랙션 차단/허용 — true: HitTestInvisible, false: Visible
	UFUNCTION(BlueprintCallable, Category = "Hand")
	void SetTargetingMode(bool bIsTargeting);

	// 호버 시 해당 카드를 최상위로 — WBP_Card의 OnCardHovered에서 호출
	UFUNCTION(BlueprintCallable, Category = "Hand|Layout")
	void SetCardZOrderTop(UCardWidget* Card);

	// 호버 해제 시 원래 z-order 복원 — WBP_Card의 OnCardUnhovered에서 호출
	UFUNCTION(BlueprintCallable, Category = "Hand|Layout")
	void RestoreCardZOrder(UCardWidget* Card);

	// ── BP 구현부 ─────────────────────────────────────────────────────────
	// WBP_HandPanel에서 카드 위젯(WBP_Card)을 생성하고 CardContainer에 추가
	UFUNCTION(BlueprintImplementableEvent, Category = "Hand")
	void OnShowHand(const TArray<FCardDataRow>& Cards);

	// 핸드 등장 애니메이션 — 플레이어 선택 시 최초 1회만 호출
	UFUNCTION(BlueprintImplementableEvent, Category = "Hand")
	void PlayShowAnimation();

	// 손패 숨김 역모션 재생 — 완료 시 ClearHand() + Hidden 처리
	UFUNCTION(BlueprintImplementableEvent, Category = "Hand")
	void PlayHideAnimation();

protected:
	// 마우스가 핸드 패널에 진입할 때 대기 카드의 호버 상태 해제
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	// 기존 HorizontalBox 컨테이너 — WBP 교체 전까지 유지 (타입 충돌 방지)
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UHorizontalBox* CardContainer;

	// 겹침 레이아웃용 캔버스 패널 — WBP에서 'CardCanvas' 이름으로 Canvas Panel 추가 필요
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UCanvasPanel* CardCanvas;

	// 뽑을 덱 카운트 텍스트 — WBP에서 이름 'Text_DrawCount'
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UTextBlock* Text_DrawCount;

	// 버린 덱 카운트 텍스트 — WBP에서 이름 'Text_DiscardCount'
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UTextBlock* Text_DiscardCount;

private:
	// OnShowHand 완료 후 카드 위치·z-order 적용
	void ApplyCardLayout();

	// 현재 대기 상태인 카드 위젯
	UPROPERTY()
	UCardWidget* PendingCard = nullptr;

	// 타겟 지정 중 여부 — NativeOnMouseEnter에서 상호작용 차단에 사용
	bool bIsTargeting = false;

	// 현재 호버 중인 카드 위젯 — 빠른 마우스 이동 시 이전 카드 강제 언호버에 사용
	UPROPERTY()
	UCardWidget* HoveredCard = nullptr;
};
