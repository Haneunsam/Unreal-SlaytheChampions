#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Card/CardDataTypes.h"
#include "Card/CardCollectionSubsystem.h"
#include "CollectionWidget.generated.h"

class UButton;
class UWrapBox;
class UScrollBox;
class UCardWidget;
class UCardStyleDataAsset;

/**
 * UCollectionWidget
 *
 * 카드 도감 전체 UI 의 C++ 베이스 클래스.
 * WBP_Collection 의 부모 클래스로 설정.
 *
 * [재사용 방법]
 * 어느 WBP 에서든 버튼 클릭 시:
 *   CreateWidget(WBP_Collection) → AddToViewport(ZOrder=10)
 * 닫기 버튼(CloseButton)을 누르면 RemoveFromParent
 *
 * [필터 동작]
 * - SetJobFilter(EJobClass) : 직업 필터 설정 → RefreshGrid 자동 호출
 * - ClearFilter()           : 필터 초기화 → 전체 카드 표시
 *
 * [카드 그리드]
 * - WrapBox(CardGrid) 에 WBP_Card 동적 생성
 * - CardWidgetClass 에 WBP_Card 지정 (에디터 디테일 패널)
 * - CardStyleAsset 에 직업별 DA_CardStyle 지정 (에디터 디테일 패널)
 */
UCLASS()
class SLAYTHECHAMPIONS_API UCollectionWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // ── 라이프사이클 ─────────────────────────────────────────────────────────────

    virtual void NativeOnInitialized() override;

    // ── 공개 API ─────────────────────────────────────────────────────────────────

    /**
     * 도감을 열고 ViewPort 에 추가한다.
     * 외부(다른 WBP 버튼 등)에서 호출.
     * 이미 열려있으면 무시.
     */
    UFUNCTION(BlueprintCallable, Category = "Collection")
    void OpenCollection();

    /**
     * 도감을 닫고 ViewPort 에서 제거한다.
     */
    UFUNCTION(BlueprintCallable, Category = "Collection")
    void CloseCollection();

    /**
     * 직업 필터를 설정하고 그리드를 갱신한다.
     * 직업 버튼 OnClicked 에 연결.
     * Any 를 넘기면 전체 표시.
     */
    UFUNCTION(BlueprintCallable, Category = "Collection")
    void SetJobFilter(EJobClass JobClass);

    /**
     * 필터를 모두 초기화하고 전체 카드를 표시한다.
     */
    UFUNCTION(BlueprintCallable, Category = "Collection")
    void ClearFilter();

    /**
     * 현재 필터 조건으로 그리드를 다시 채운다.
     * SetJobFilter / ClearFilter 내부에서 자동 호출.
     */
    UFUNCTION(BlueprintCallable, Category = "Collection")
    void RefreshGrid();

    // ── Blueprint 이벤트 ────────────────────────────────────────────────────────

    /** 도감이 열릴 때 BP 에 알림 */
    UFUNCTION(BlueprintImplementableEvent, Category = "Collection")
    void OnCollectionOpened();

    /** 도감이 닫힐 때 BP 에 알림 */
    UFUNCTION(BlueprintImplementableEvent, Category = "Collection")
    void OnCollectionClosed();

    /** 그리드 갱신 완료 시 BP 에 알림 */
    UFUNCTION(BlueprintImplementableEvent, Category = "Collection")
    void OnGridRefreshed(int32 CardCount);

protected:
    // ── BindWidget (WBP_Collection 에서 같은 이름으로 배치 필수) ───────────────

    // 카드 그리드 — 카드가 가로로 자동 줄바꿈
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UWrapBox> CardGrid;

    // 스크롤 박스 — CardGrid 를 감싸서 세로 스크롤 지원
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UScrollBox> CardScrollBox;

    // 닫기 버튼
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UButton> CloseButton;

    // ── 에디터 설정 (디테일 패널에서 지정) ────────────────────────────────────

    // WBP_Card 클래스 — 에디터에서 WBP_Card 지정
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collection|Setup")
    TSubclassOf<UCardWidget> CardWidgetClass;

    // 카드 스타일 에셋 — 에디터에서 DA_CardStyle 지정 (없으면 스타일 없이 표시)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collection|Setup")
    TObjectPtr<UCardStyleDataAsset> CardStyleAsset;

private:
    UFUNCTION()
    void OnCloseButtonClicked();

    // 현재 적용 중인 필터
    UPROPERTY()
    FCardCollectionFilter CurrentFilter;
};
