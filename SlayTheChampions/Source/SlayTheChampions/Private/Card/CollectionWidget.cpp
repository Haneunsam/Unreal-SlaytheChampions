#include "Card/CollectionWidget.h"
#include "Card/CardWidget.h"
#include "Card/CardStyleDataAsset.h"
#include "Card/CardCollectionSubsystem.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "Components/ScrollBox.h"

// ── 라이프사이클 ──────────────────────────────────────────────────────────────

void UCollectionWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (CloseButton)
        CloseButton->OnClicked.AddDynamic(this, &UCollectionWidget::OnCloseButtonClicked);
}

// ── 공개 API ──────────────────────────────────────────────────────────────────

void UCollectionWidget::OpenCollection()
{
    if (IsInViewport()) return;

    AddToViewport(10);
    RefreshGrid();
    OnCollectionOpened();
}

void UCollectionWidget::CloseCollection()
{
    OnCollectionClosed();
    RemoveFromParent();
}

void UCollectionWidget::SetJobFilter(EJobClass JobClass)
{
    CurrentFilter.JobClasses.Reset();

    // Any 는 전체 표시 (필터 없음)
    if (JobClass != EJobClass::Any)
        CurrentFilter.JobClasses.Add(JobClass);

    RefreshGrid();
}

void UCollectionWidget::ClearFilter()
{
    CurrentFilter = FCardCollectionFilter();
    RefreshGrid();
}

void UCollectionWidget::RefreshGrid()
{
    if (!CardGrid)
    {
        UE_LOG(LogTemp, Warning, TEXT("[CollectionWidget] RefreshGrid - CardGrid is null."));
        return;
    }
    if (!CardWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("[CollectionWidget] RefreshGrid - CardWidgetClass is not set. 에디터 디테일 패널에서 WBP_Card 를 지정하세요."));
        return;
    }

    // 기존 카드 전부 제거
    CardGrid->ClearChildren();

    // 필터 적용 카드 목록 가져오기
    UCardCollectionSubsystem* CollectionSS =
        GetGameInstance()->GetSubsystem<UCardCollectionSubsystem>();
    if (!CollectionSS)
    {
        UE_LOG(LogTemp, Warning, TEXT("[CollectionWidget] RefreshGrid - CardCollectionSubsystem not found."));
        return;
    }

    TArray<FCardDataRow> Cards = CollectionSS->GetFilteredCards(CurrentFilter);

    for (const FCardDataRow& CardData : Cards)
    {
        UCardWidget* CardWidget = CreateWidget<UCardWidget>(GetOwningPlayer(), CardWidgetClass);
        if (!CardWidget) continue;

        // WBP_Card 의 SetCardData 호출 (스타일은 없으면 nullptr 전달)
        CardWidget->SetCardData(CardData, CardStyleAsset);
        CardGrid->AddChildToWrapBox(CardWidget);
    }

    OnGridRefreshed(Cards.Num());

    UE_LOG(LogTemp, Log, TEXT("[CollectionWidget] RefreshGrid - %d cards displayed."), Cards.Num());
}

// ── 내부 ──────────────────────────────────────────────────────────────────────

void UCollectionWidget::OnCloseButtonClicked()
{
    CloseCollection();
}
