#include "Party/PartyInstance.h"

#include "Relic/RelicSubsystem.h"
#include "Unit/Unit.h"
#include "Unit/StatComponent.h"

void UPartyInstance::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (PartyInfo.Champions.IsEmpty())
	{
		InitParty();
	}
}

void UPartyInstance::InitParty()
{
	PartyInfo.InitSavePartyInfo();
	PartyInfo.Deck.SetNum(2);
}

void UPartyInstance::SetPartyInfo(FSavePartyInfo _info)
{
	PartyInfo = _info;
	PartyInfo.Champions.Empty();
}

int32 UPartyInstance::GetPartyMemberCount() const
{
	return PartyInfo.Champions.Num() > 0 ? PartyInfo.Champions.Num() : PartyInfo.PartyMemberIDs.Num();
}

void UPartyInstance::RegisterChampion(AUnit* Unit)
{
	const int32 BeforeCount = PartyInfo.Champions.Num();

	if (Unit)
	{
		PartyInfo.Champions.AddUnique(Unit);
		if (!Unit->UnitID.IsNone())
		{
			PartyInfo.PartyMemberIDs.AddUnique(Unit->UnitID);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[PartyInstance] RegisterChampion Unit=%s UnitID=%s Before=%d After=%d SavedIDs=%d"),
		Unit ? *Unit->GetName() : TEXT("None"),
		Unit ? *Unit->UnitID.ToString() : TEXT("None"),
		BeforeCount,
		PartyInfo.Champions.Num(),
		PartyInfo.PartyMemberIDs.Num());
}


void UPartyInstance::AddGold(int32 _Gold)
{
	UE_LOG(LogTemp, Warning, TEXT("골드획득 : %d"), _Gold);
	PartyInfo.Gold += _Gold;
	if (PartyInfo.Gold >= PartyInfo.MaxGold)
		PartyInfo.Gold = PartyInfo.MaxGold;

	UE_LOG(LogTemp, Warning, TEXT("현재 골드 : %d"), PartyInfo.Gold);
}

void UPartyInstance::UseGold(int32 _Price)
{
	UE_LOG(LogTemp, Warning, TEXT("골드사용 : %d"), _Price);
	if (PartyInfo.Gold >= _Price)
	{
		PartyInfo.Gold -= _Price;
	}

	if (PartyInfo.Gold < 0) PartyInfo.Gold = 0;
	UE_LOG(LogTemp, Warning, TEXT("현재 골드 : %d"), PartyInfo.Gold);
}

void UPartyInstance::AddRelic(FRelic _Relic)
{
	UE_LOG(LogTemp, Warning, TEXT("유물추가 : %s"), *_Relic.RelicID.ToString());
	PartyInfo.Relics.Add(_Relic);
	if (PartyInfo.Relics.ContainsByPredicate([_Relic](const FRelic& Item)
	{
		return Item.RelicID == _Relic.RelicID;
	}))
	{
		UE_LOG(LogTemp, Warning, TEXT("유물획득성공 : %s"), *_Relic.RelicID.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("유물획득실패"));
	}

	if (URelicSubsystem* RelicSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<URelicSubsystem>() : nullptr)
	{
		RelicSubsystem->TriggerRelicEffectsByTiming(_Relic, EEffectApplyTiming::OnAcquire, PartyInfo.Champions);
	}
	
}

void UPartyInstance::LostRelic(FName _RelicName) 
{
	PartyInfo.Relics.RemoveAll([_RelicName](const FRelic& Item)
	{
		return Item.RelicID == _RelicName;
	});
}

void UPartyInstance::AddPotion(FPotionData _Potion)
{
	PartyInfo.Potions.Add(_Potion);
}

void UPartyInstance::LostPotion(FName _PotionName)
{
	PartyInfo.Potions.RemoveAll([_PotionName](const FPotionData& Item)
	{
		return Item.PotionID == _PotionName;
	});
}

void UPartyInstance::SaveChampionHPs(const TArray<AUnit*>& Units)
{
	PartyInfo.ChampionCurrentHPs.SetNum(Units.Num());
	PartyInfo.ChampionMaxHPs.SetNum(Units.Num());
	for (int32 i = 0; i < Units.Num(); i++)
	{
		if (UStatComponent* Stat = Units[i] ? Units[i]->FindComponentByClass<UStatComponent>() : nullptr)
		{
			PartyInfo.ChampionCurrentHPs[i] = Stat->CurrentHP;
			PartyInfo.ChampionMaxHPs[i]     = Stat->MaxHP;
		}
	}
	UE_LOG(LogTemp, Log, TEXT("[PartyInstance] SaveChampionHPs: %d명 HP 저장"), Units.Num());
}

int32 UPartyInstance::GetSavedCurrentHP(int32 Index) const
{
	return PartyInfo.ChampionCurrentHPs.IsValidIndex(Index) ? PartyInfo.ChampionCurrentHPs[Index] : 0;
}

int32 UPartyInstance::GetSavedMaxHP(int32 Index) const
{
	return PartyInfo.ChampionMaxHPs.IsValidIndex(Index) ? PartyInfo.ChampionMaxHPs[Index] : 0;
}


