#include "Party/PartyInstance.h"

#include "Relic/RelicSubsystem.h"
#include "Unit/Unit.h"

namespace
{
FString MakePartyMemberLogString(const TArray<FName>& PartyMemberIDs)
{
	TArray<FString> MemberNames;
	for (const FName& PartyMemberID : PartyMemberIDs)
	{
		MemberNames.Add(PartyMemberID.ToString());
	}

	return FString::Join(MemberNames, TEXT(", "));
}
}

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

bool UPartyInstance::AddPartyMember(FName UnitID, EJobClass Job)
{
	if (UnitID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[PartyInstance] AddPartyMember failed. UnitID is None."));
		return false;
	}

	const int32 BeforeIDCount = PartyInfo.PartyMemberIDs.Num();
	PartyInfo.PartyMemberIDs.AddUnique(UnitID);
	if (PartyInfo.PartyMemberIDs.Num() == BeforeIDCount)
	{
		const FString PartyMembers = MakePartyMemberLogString(PartyInfo.PartyMemberIDs);
		UE_LOG(LogTemp, Warning, TEXT("[PartyInstance] AddPartyMember skipped. Already exists UnitID=%s CurrentPartyCount=%d PartyMembers=[%s]"),
			*UnitID.ToString(),
			PartyInfo.PartyMemberIDs.Num(),
			*PartyMembers);
		return false;
	}

	if (Job != EJobClass::Any)
	{
		ChampionJobs.Add(Job);
	}

	const FString PartyMembers = MakePartyMemberLogString(PartyInfo.PartyMemberIDs);
	UE_LOG(LogTemp, Warning, TEXT("[PartyInstance] AddPartyMember success. UnitID=%s Job=%s IDs %d->%d Jobs=%d PartyMembers=[%s]"),
		*UnitID.ToString(),
		*StaticEnum<EJobClass>()->GetNameStringByValue(static_cast<int64>(Job)),
		BeforeIDCount,
		PartyInfo.PartyMemberIDs.Num(),
		ChampionJobs.Num(),
		*PartyMembers);

	return true;
}

bool UPartyInstance::RemovePartyMember(FName UnitID, EJobClass Job)
{
	if (UnitID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[PartyInstance] RemovePartyMember failed. UnitID is None."));
		return false;
	}

	const int32 BeforeIDCount = PartyInfo.PartyMemberIDs.Num();
	const int32 RemovedIDCount = PartyInfo.PartyMemberIDs.Remove(UnitID);
	if (RemovedIDCount <= 0)
	{
		const FString PartyMembers = MakePartyMemberLogString(PartyInfo.PartyMemberIDs);
		UE_LOG(LogTemp, Warning, TEXT("[PartyInstance] RemovePartyMember skipped. Missing UnitID=%s CurrentPartyCount=%d PartyMembers=[%s]"),
			*UnitID.ToString(),
			PartyInfo.PartyMemberIDs.Num(),
			*PartyMembers);
		return false;
	}

	if (Job != EJobClass::Any)
	{
		ChampionJobs.RemoveSingle(Job);
	}

	PartyInfo.Champions.RemoveAll([UnitID](const AUnit* Unit)
	{
		return Unit && Unit->UnitID == UnitID;
	});

	const FString PartyMembers = MakePartyMemberLogString(PartyInfo.PartyMemberIDs);
	UE_LOG(LogTemp, Warning, TEXT("[PartyInstance] RemovePartyMember success. UnitID=%s Job=%s IDs %d->%d Jobs=%d PartyMembers=[%s]"),
		*UnitID.ToString(),
		*StaticEnum<EJobClass>()->GetNameStringByValue(static_cast<int64>(Job)),
		BeforeIDCount,
		PartyInfo.PartyMemberIDs.Num(),
		ChampionJobs.Num(),
		*PartyMembers);

	return true;
}

bool UPartyInstance::HasPartyMember(FName UnitID) const
{
	return !UnitID.IsNone() && PartyInfo.PartyMemberIDs.Contains(UnitID);
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


