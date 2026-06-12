// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit/Job/JobComponent.h"
#include "Unit/Job/JobDetail.h"
#include "Unit/Job/Job_Healer.h"
#include "Unit/Job/Job_Mage.h"
#include "Unit/Job/Job_Warrior.h"
#include "Unit/UnitAnimComponent.h"
#include "Unit/Job/JobVisualDataAsset.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values for this component's properties
UJobComponent::UJobComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UJobComponent::BeginPlay()
{
	Super::BeginPlay();

	//JobClass 드롭다운에 맞는 메시/애니메이션 세트 주입
	ApplyJobVisual();
	//직업로직 생성
	SpawnDetail();
}


// 런타임에 직업 변경 — JobClass 갱신 후 Detail 재생성
void UJobComponent::SetJobClass(EJobClass NewJob)
{
	JobClass = NewJob;
	Detail = nullptr;   // 이전 직업 Detail 폐기 (GC 대상) 후 새로 생성
	ApplyJobVisual();
	SpawnDetail();
}

//
void UJobComponent::SpawnDetail()
{
	TSubclassOf<UJobDetail> DetailClass = nullptr;

	switch (JobClass)
	{
	
	case EJobClass::Warrior: DetailClass = UJob_Warrior::StaticClass(); break;
	case EJobClass::Mage:    DetailClass = UJob_Mage::StaticClass();    break;
	case EJobClass::Healer:  DetailClass = UJob_Healer::StaticClass();  break;
	default:
		// EJobClass::Any
		UE_LOG(LogTemp, Warning,
			TEXT("[JobComponent] JobClass=Any: no detail spawned (%s)"),
			*GetOwner()->GetName());
		return;
	}

	Detail = NewObject<UJobDetail>(this, DetailClass);
	if (Detail)
	{
		Detail->Initialize(this);
		UE_LOG(LogTemp, Log,
			TEXT("[JobComponent] Spawned %s for %s"),
			*DetailClass->GetName(), *GetOwner()->GetName());
	}
}


void UJobComponent::OnCardPlayed(FGameplayTag CardTag, int32 CardValue)
{
	if (Detail)
	{
		Detail->OnCardPlayed(CardTag, CardValue);
	}
}

float UJobComponent::ModifyCardDamage(FGameplayTag CardTag, float BaseDamage)
{
	if (Detail)
	{
		return Detail->ModifyCardDamage(CardTag, BaseDamage);
	}
	return BaseDamage;
}

void UJobComponent::OnTurnStart()
{
	if (Detail)
	{
		Detail->OnTurnStart();
	}
}
void UJobComponent::OnTurnEnd()
{
	if (Detail)
	{
		Detail->OnTurnEnd();
	}
}
//비주얼 적용
void UJobComponent::ApplyJobVisual()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	//JobClass에 대응하는 비주을 DA조회, 없으면 에디터에서 세팅된 메시 그대로 두기
	const TObjectPtr<UJobVisualDataAsset>* Found = JobVisuals.Find(JobClass);
	if (!Found || !(*Found)) return;
	const UJobVisualDataAsset* Visual = *Found;

	//대상 메시 결정
	if (!TargetMesh)
	{
		TargetMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
	}
	//메시 교체
	if (TargetMesh)
	{
		if (Visual->SkeletalMesh)
			TargetMesh->SetSkeletalMesh(Visual->SkeletalMesh);

		if (Visual->AnimBPClass)
			TargetMesh->SetAnimInstanceClass(Visual->AnimBPClass);

		TargetMesh->SetRelativeLocation(Visual->MeshOffset);
		TargetMesh->SetRelativeScale3D(Visual->MeshScale);
		TargetMesh->SetRelativeRotation(Visual->MeshRotation);
	}
	//몽타주 세트 주입
	if (Visual->AnimData)
	{
		if (UUnitAnimComponent* Anim = Owner->FindComponentByClass<UUnitAnimComponent>())
			Anim->AnimData = Visual->AnimData;
	}
}



