#include "Reward/BattleClearWidget.h"
#include "Relic/RelicSubsystem.h"
#include "Map/RunSystem.h"
#include "Reward/RewardStruct.h"

void UBattleClearWidget::NativeConstruct()
{
    Super::NativeConstruct();

    OnVisibilityChanged.AddUniqueDynamic(this, &UBattleClearWidget::HandleVisibilityChanged);
}

TArray<FRewardData> UBattleClearWidget::GenerateRewardData(EAreaType _type)
{
    TArray<FRewardData> Rewards;

    auto AddReward = [&Rewards](ERewardTypes RewardType, int32 Amount = 0, FName RewardId = NAME_None)
    {
        FRewardData RewardData;
        RewardData.RewardType = RewardType;
        RewardData.Amount = Amount;
        RewardData.RewardId = RewardId;
        Rewards.Add(RewardData);
    };

    // 추후 유물 추가할 때 if문으로 골드나 NomalCard 등을 더 설정
    switch (_type)
    {
    case EAreaType::Normal:
        AddReward(ERewardTypes::NomalCard, 0, TEXT("NormalCardReward"));
        AddReward(ERewardTypes::Gold, RollGold(_type), TEXT("Gold"));

        if (RollChance(PotionRewardChances::Normal))
        {
            AddReward(ERewardTypes::Potion, 1, TEXT("Potion"));
        }
        break;

    case EAreaType::Elite:
        AddReward(ERewardTypes::NomalCard, 0, TEXT("NormalCardReward"));
        AddReward(ERewardTypes::Gold, RollGold(_type), TEXT("Gold"));
        if (UGameInstance* GameInstance = GetGameInstance())
        {
            if (URelicSubsystem* RelicSubsystem = GameInstance->GetSubsystem<URelicSubsystem>())
            {
                const FName RandomRelicId = RelicSubsystem->GetRandomCommonRelic();
                AddReward(ERewardTypes::Relic, 1, RandomRelicId);
            }
        }

        if (RollChance(PotionRewardChances::Elite))
        {
            AddReward(ERewardTypes::Potion, 1, TEXT("Potion"));
        }
        break;

    case EAreaType::Boss:
        AddReward(ERewardTypes::LegendCard, 0, TEXT("LegendCardReward"));
        AddReward(ERewardTypes::Gold, RollGold(_type), TEXT("Gold"));

        if (RollChance(PotionRewardChances::Boss))
        {
            AddReward(ERewardTypes::Potion, 1, TEXT("Potion"));
        }
        break;
        
    default:
        break;
    }

    return Rewards;
}

void UBattleClearWidget::RefreshRewards()
{
    EAreaType AreaType = EAreaType::Normal;

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (URunSystem* RunSystem = GameInstance->GetSubsystem<URunSystem>())
        {
            AreaType = RunSystem->GetCurrentRoomInfo().AreaType;
        }
    }

    CurrentRewardData = GenerateRewardData(AreaType);
    OnRewardDataRefreshed(CurrentRewardData);
}

void UBattleClearWidget::HandleVisibilityChanged(ESlateVisibility InVisibility)
{
    if (InVisibility == ESlateVisibility::Visible ||
        InVisibility == ESlateVisibility::HitTestInvisible ||
        InVisibility == ESlateVisibility::SelfHitTestInvisible)
    {
        RefreshRewards();
    }
}

bool UBattleClearWidget::RollChance(float _Probability)
{
    const float Rand = FMath::FRandRange(1.f, 100.f);
    return Rand <= _Probability;
}

int32 UBattleClearWidget::RollGold(EAreaType _type)
{
    int32 gold = 0;
    switch (_type)
    {
    case EAreaType::Normal:
        gold = FMath::RandRange((int32)EGoldAmount::Min_Gold, (int32)EGoldAmount::Nomal_Gold);
        break;

    case EAreaType::Elite:
        gold = FMath::RandRange((int32)EGoldAmount::Nomal_Gold, (int32)EGoldAmount::Elite_Gold);
        break;

    case EAreaType::Boss:
        gold = FMath::RandRange((int32)EGoldAmount::Elite_Gold, (int32)EGoldAmount::Boss_Gold);
        break;
    }
    return gold;
}
