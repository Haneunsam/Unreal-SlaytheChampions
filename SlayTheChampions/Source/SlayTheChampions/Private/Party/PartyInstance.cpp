#include "Party/PartyInstance.h"

#include "Relic/RelicSubsystem.h"
#include "Unit/Unit.h"
#include "Unit/StatComponent.h"
#include "Card/CardSubsystem.h"

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

void UPartyInstance::AddChampion(EJobClass Job)
{
	const int32 PawnIndex = ChampionJobs.Num();
	ChampionJobs.Add(Job);

	// 직업 기본 덱을 인스턴스에 함께 저장 — 이후 전투/보상은 이 덱을 소스로 사용
	if (UCardSubsystem* CS = GetGameInstance() ? GetGameInstance()->GetSubsystem<UCardSubsystem>() : nullptr)
	{
		TArray<FName> BaseDeck = CS->GetCardNamesByClass(Job);
		SetDeck(PawnIndex, BaseDeck);
		UE_LOG(LogTemp, Log, TEXT("[PartyInstance] AddChampion Pawn%d Job=%d 기본덱 %d장 시드"),
			PawnIndex, (int32)Job, BaseDeck.Num());
	}
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

void UPartyInstance::HealAllChampionsByPercent(float Percent)
{
	for (int32 i = 0; i < PartyInfo.ChampionCurrentHPs.Num(); ++i)
	{
		const int32 MaxHP = PartyInfo.ChampionMaxHPs.IsValidIndex(i) ? PartyInfo.ChampionMaxHPs[i] : 0;
		if (MaxHP <= 0) continue;
		const int32 HealAmount = FMath::CeilToInt(MaxHP * Percent);
		PartyInfo.ChampionCurrentHPs[i] = FMath::Min(PartyInfo.ChampionCurrentHPs[i] + HealAmount, MaxHP);
		UE_LOG(LogTemp, Log, TEXT("[PartyInstance] Heal Pawn%d +%d → %d/%d"),
			i, HealAmount, PartyInfo.ChampionCurrentHPs[i], MaxHP);
	}
}

void UPartyInstance::HealAllChampions(int32 Amount)
{
	for (int32 i = 0; i < PartyInfo.ChampionCurrentHPs.Num(); ++i)
	{
		const int32 MaxHP = PartyInfo.ChampionMaxHPs.IsValidIndex(i) ? PartyInfo.ChampionMaxHPs[i] : 0;
		if (MaxHP <= 0) continue;
		PartyInfo.ChampionCurrentHPs[i] = FMath::Min(PartyInfo.ChampionCurrentHPs[i] + Amount, MaxHP);
	}
}

void UPartyInstance::SetDeck(int32 PawnIndex, const TArray<FName>& Cards)
{
	if (PawnIndex < 0) return;
	if (!PartyInfo.ChampionDecks.IsValidIndex(PawnIndex))
		PartyInfo.ChampionDecks.SetNum(PawnIndex + 1);
	PartyInfo.ChampionDecks[PawnIndex].Cards = Cards;
	UE_LOG(LogTemp, Log, TEXT("[PartyInstance] SetDeck: Pawn%d = %d장"), PawnIndex, Cards.Num());
}

void UPartyInstance::AddDeckCard(int32 PawnIndex, FName CardName)
{
	if (PawnIndex < 0 || CardName.IsNone()) return;

	// 덱은 RowName 체계로 통일 — CardName이 CardID면 RowName으로 변환해 저장
	// (기본 덱은 GetCardNamesByClass로 RowName을 쓰므로, 보상 카드도 맞춰야 손패/드로우에서 GetCard 성공)
	if (UCardSubsystem* CS = GetGameInstance() ? GetGameInstance()->GetSubsystem<UCardSubsystem>() : nullptr)
	{
		if (!CS->GetCard(CardName))
		{
			const FName RowName = CS->GetRowNameByCardID(CardName);
			if (!RowName.IsNone()) CardName = RowName;
		}
	}

	if (!PartyInfo.ChampionDecks.IsValidIndex(PawnIndex))
		PartyInfo.ChampionDecks.SetNum(PawnIndex + 1);
	PartyInfo.ChampionDecks[PawnIndex].Cards.Add(CardName);
	UE_LOG(LogTemp, Log, TEXT("[PartyInstance] AddDeckCard: Pawn%d += %s"), PawnIndex, *CardName.ToString());
}

TArray<FName> UPartyInstance::GetDeck(int32 PawnIndex) const
{
	if (!PartyInfo.ChampionDecks.IsValidIndex(PawnIndex)) return {};
	return PartyInfo.ChampionDecks[PawnIndex].Cards;
}

bool UPartyInstance::HasDeck(int32 PawnIndex) const
{
	return PartyInfo.ChampionDecks.IsValidIndex(PawnIndex) &&
		   PartyInfo.ChampionDecks[PawnIndex].Cards.Num() > 0;
}


