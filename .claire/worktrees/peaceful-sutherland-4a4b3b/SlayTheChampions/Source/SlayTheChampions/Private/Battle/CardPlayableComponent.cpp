#include "Battle/CardPlayableComponent.h"
#include "Battle/DeckComponent.h"

UCardPlayableComponent::UCardPlayableComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCardPlayableComponent::BeginPlay()
{
    Super::BeginPlay();
    DeckComp = GetOwner()->FindComponentByClass<UDeckComponent>();
}

bool UCardPlayableComponent::CanPlayCard(FName CardId, int32 AvailableCost) const
{
    if (!DeckComp) return false;

    const TArray<FCardData>& Hand = DeckComp->GetHand();
    const FCardData* Found = Hand.FindByPredicate([&](const FCardData& Card)
    {
        return Card.CardId == CardId;
    });

    return Found && AvailableCost >= Found->Cost;
}

bool UCardPlayableComponent::TryPlayCard(FName CardId, int32 AvailableCost, const TArray<FName>& TargetIds, int32 InputIndex, FActionEvent& OutEvent)
{
    if (!CanPlayCard(CardId, AvailableCost)) return false;

    FCardData PlayedCard;
    if (!DeckComp->PlayCard(CardId, PlayedCard)) return false;

    OutEvent.EventId = FGuid::NewGuid();
    OutEvent.SourceCardId = PlayedCard.CardId;
    OutEvent.ActorId = OwnerId;
    OutEvent.TargetIds = TargetIds;
    OutEvent.EventType = PlayedCard.EventType;
    OutEvent.EffectPayload = BuildEffectPayload(PlayedCard);
    OutEvent.Tags = PlayedCard.Tags;
    OutEvent.InputIndex = InputIndex;
    OutEvent.QueueIndex = QueueIndexCounter++;
    OutEvent.bIsUpgradedToUltimate = false;

    return true;
}

FEffectPayload UCardPlayableComponent::BuildEffectPayload(const FCardData& Card) const
{
    FEffectPayload Payload;
    for (const FEffectEntry& Entry : Card.EffectList)
    {
        switch (Entry.EffectType)
        {
        case EEffectType::Damage:    Payload.DamageAmount += Entry.Value; break;
        case EEffectType::Block:     Payload.BlockAmount  += Entry.Value; break;
        case EEffectType::Heal:      Payload.HealAmount   += Entry.Value; break;
        case EEffectType::DrawCard:  Payload.DrawCount    += Entry.Value; break;
        case EEffectType::ApplyStatus: Payload.StatusesToApply.Add(Entry.StatusId); break;
        }
    }
    return Payload;
}
