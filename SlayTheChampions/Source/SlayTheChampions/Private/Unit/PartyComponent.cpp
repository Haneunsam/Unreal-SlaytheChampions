// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/PartyComponent.h"
#include "Unit/Unit.h"
#include "Unit/StatComponent.h"


// Sets default values for this component's properties
UPartyComponent::UPartyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPartyComponent::BeginPlay()
{
	Super::BeginPlay();
}


//-----------로스터 관리------------
// [델리게이트 정리 정책]
//  - EndPlay/맵 전환 시 수동 UnbindMember 를 하지 않는다.
//    AUnit::OnUnitDied(다이내믹 멀티캐스트)는 리스너(this)를 FWeakObjectPtr 로 잡으므로,
//    이 컴포넌트가 파괴되면 엔진이 무효 바인딩을 자동으로 건너뛴다(댕글링 호출 없음).
//  - 반면 UnregisterMember / ClearParty 에서는 Unbind 를 "유지"한다.
//    이쪽은 유닛이 살아있는데도 더 이상 이 파티에 알리면 안 되는 경우라,
//    GC 자동정리로는 해결되지 않는 "로직 정확성" 문제이기 때문이다.
int32 UPartyComponent::RegisterMember(AUnit* Unit)
{
	if (!IsValid(Unit))
	{
		return INDEX_NONE;
	}
	//중복등록방지 이미 있으면 기존 인덱스 반환
	const int32 Existing = GetMemberIndex(Unit);
	if (Existing != INDEX_NONE)
	{
		return Existing;
	}
	if (Members.Num() >= MaxMembers)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PartyComponent] 파티가 가득 찼습니다 (Max %d). 등록 실패: %s"),
			MaxMembers, *GetNameSafe(Unit));
		return INDEX_NONE;
	}
	const int32 NewIndex = Members.Add(Unit);
	BindMember(Unit);
	// 첫 멤버는 자동으로 선택 슬롯으로 지정 (아군 파티 UI 편의).
	if (SelectedIndex == INDEX_NONE && Unit->IsAlive())
	{
		SelectMember(NewIndex);
	}

	OnMemberRegistered.Broadcast(Unit, NewIndex);
	return NewIndex;
}

void UPartyComponent::UnregisterMember(AUnit* Unit)
{
	const int32 Index = GetMemberIndex(Unit);
	if (Index == INDEX_NONE)
	{
		return;
	}

	UnbindMember(Unit);
	Members[Index] = nullptr; //슬롯은 유지하되 비운다.
	//선택슬롯이 비어있으면 다음 살아있는 멤버로 간다.
	if (SelectedIndex == Index)
	{
		AUnit* Next = GetRandomAliveMember();
		SelectUnit(Next); // Next 가 null 이면 내부에서 선택 해제 처리.
	}
}

// [인덱스 안정성 정책]
//  - 슬롯 인덱스는 "한 전투 세션 내"에서만 고정된다.
//    세션 중 멤버가 죽거나 UnregisterMember 되면 슬롯은 비워질 뿐(nullptr) 인덱스는 유지된다.
//  - ClearParty 는 세션 자체를 끝내는 작업이라 배열을 Reset() 해 인덱스를 무효화한다.
//    이후 다음 전투에서 RegisterMember 로 0번부터 다시 채운다.
//  - 외부 인덱스 접근은 항상 GetMember(Index) 를 거치며, 이 함수는 범위 밖이면
//    크래시 대신 nullptr 을 반환하므로 ClearParty 후 접근해도 out-of-bounds 가 나지 않는다.

void UPartyComponent::ClearParty()
{
	for (const TObjectPtr<AUnit>& Member : Members)
	{
		UnbindMember(Member.Get());
	}
	Members.Reset();

	const int32 Old = SelectedIndex;
	SelectedIndex = INDEX_NONE;
	if (Old != INDEX_NONE)
	{
		OnSelectionChanged.Broadcast(Old, INDEX_NONE, nullptr);
	}
}

//------------------조회--------------------

AUnit* UPartyComponent::GetMember(int32 Index) const
{
	return Members.IsValidIndex(Index) ? Members[Index].Get() : nullptr;
}

int32 UPartyComponent::GetMemberIndex(const AUnit* Unit) const
{
	if (!Unit)
	{
		return INDEX_NONE;
	}
	for (int32 i = 0; i < Members.Num(); ++i)
	{
		if (Members[i].Get() == Unit)
		{
			return i;
		}
	}
	return INDEX_NONE;
}


// 등록된 실제 멤버(생존 + 사망)만 반환한다. 빈 슬롯(nullptr)은 멤버가 아니므로 제외.
// 주의: 반환 배열의 인덱스는 슬롯 인덱스와 일치하지 않을 수 있다(빈 슬롯이 빠지므로).
//       슬롯 인덱스 기준 접근이 필요하면 GetMember(Index) 를 사용할 것.

TArray<AUnit*> UPartyComponent::GetAllMembers() const
{
	TArray<AUnit*> Out;
	Out.Reserve(Members.Num());
	for (const TObjectPtr<AUnit>& Member : Members)
	{
		AUnit* U = Member.Get();
		if (IsValid(U))   // 빈 슬롯·파괴 대기 객체 제외
		{
			Out.Add(U);
		}
	}
	return Out;
}

TArray<AUnit*> UPartyComponent::GetAliveMembers() const
{
	TArray<AUnit*> Out;
	for (const TObjectPtr<AUnit>& Member : Members)
	{
		AUnit* U = Member.Get();
		if (IsValid(U) && U->IsAlive())
		{
			Out.Add(U);
		}
	}
	return Out;
}

int32 UPartyComponent::GetAliveCount() const
{
	int32 Count = 0;
	for (const TObjectPtr<AUnit>& Member : Members)
	{
		AUnit* U = Member.Get();
		if (IsValid(U) && U->IsAlive())
		{
			++Count;
		}
	}
	return Count;
}

bool UPartyComponent::IsWiped() const
{
	// 등록 멤버가 하나도 없으면 "전멸"이 아니라 "아직 미구성".
	if (Members.Num() == 0)
	{
		return false;
	}
	return GetAliveCount() == 0;
}

//-----------선택------------------
bool UPartyComponent::SelectMember(int32 Index)
{
	AUnit* Target = GetMember(Index);
	if (!IsValid(Target) || !Target->IsAlive())
	{
		return false;
	}
	if (SelectedIndex == Index)
	{
		return true; // 이미 선택됨.
	}

	const int32 OldIndex = SelectedIndex;
	SelectedIndex = Index;
	OnSelectionChanged.Broadcast(OldIndex, SelectedIndex, Target);
	return true;
}

bool UPartyComponent::SelectUnit(AUnit* Unit)
{
	if (!Unit)
	{
		// 선택 해제.
		if (SelectedIndex != INDEX_NONE)
		{
			const int32 Old = SelectedIndex;
			SelectedIndex = INDEX_NONE;
			OnSelectionChanged.Broadcast(Old, INDEX_NONE, nullptr);
		}
		return false;
	}
	return SelectMember(GetMemberIndex(Unit));
}

AUnit* UPartyComponent::GetSelectedMemer() const
{
	return GetMember(SelectedIndex);
}

//----------------타겟 선정--------------
AUnit* UPartyComponent::GetRandomAliveMember() const
{
	// Reservoir sampling(k=1): 새 배열 할당 없이 단일 순회로 균등 무작위 1명 선택.
// i번째 생존자를 1/i 확률로 채택하면 최종적으로 모든 생존자가 동일 확률이 된다.
	AUnit* Chosen = nullptr;
	int32 AliveSeen = 0;
	for (const TObjectPtr<AUnit>& Member : Members)
	{
		AUnit* U = Member.Get();
		if (IsValid(U) && U->IsAlive())
		{
			++AliveSeen;
			if (FMath::RandRange(1, AliveSeen) == 1)
			{
				Chosen = U;
			}
		}
	}
	return Chosen;
}

AUnit* UPartyComponent::GetLowestHPMember() const
{
	AUnit* Best = nullptr;
	float BestRatio = TNumericLimits<float>::Max();

	for (const TObjectPtr<AUnit>& Member : Members)
	{
		AUnit* U = Member.Get();
		if (!IsValid(U) || !U->IsAlive())
		{
			continue;
		}
		const UStatComponent* Stat = U->GetStat();
		if (!Stat || Stat->MaxHP <= 0)
		{
			continue;
		}
		const float Ratio = static_cast<float>(Stat->CurrentHP) / static_cast<float>(Stat->MaxHP);
		if (Ratio < BestRatio)
		{
			BestRatio = Ratio;
			Best = U;
		}
	}
	return Best;
}


//---------------턴진행---------
void UPartyComponent::BroadcastTurnStart()
{
	for (AUnit* U : GetAliveMembers())
	{
		OnTurnStartPerMember.Broadcast(U);
	}
}

void UPartyComponent::BroadcastTurnEnd()
{
	for (AUnit* U : GetAliveMembers())
	{
		OnTurnEndPerMember.Broadcast(U);
	}
}


//----------내부---------------

void UPartyComponent::HandleMemberDied(AUnit* Unit)
{
	const int32 Index = GetMemberIndex(Unit);

	// 슬롯은 유지(인덱스 안정성). 살아있음 여부는 IsAlive() 로 판단된다.
	OnMemberDied.Broadcast(Unit, Index);

	// 선택돼 있던 멤버가 죽으면 다른 생존자로 자동 이동.
	if (SelectedIndex == Index)
	{
		AUnit* Next = GetRandomAliveMember();
		SelectUnit(Next);
	}

	// 전멸 판정.
	if (IsWiped())
	{
		OnPartyWiped.Broadcast();
	}
}

void UPartyComponent::BindMember(AUnit* Unit)
{
	if (IsValid(Unit))
	{
		// AddUniqueDynamic: 이미 바인딩돼 있으면 중복 추가하지 않는다.
		Unit->OnUnitDied.AddUniqueDynamic(this, &UPartyComponent::HandleMemberDied);
	}
}

void UPartyComponent::UnbindMember(AUnit* Unit)
{
	if (IsValid(Unit))
	{
		Unit->OnUnitDied.RemoveDynamic(this, &UPartyComponent::HandleMemberDied);
	}
}

