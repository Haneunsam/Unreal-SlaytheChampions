// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Card/CardDataTypes.h"   // EJobClass

#include "JobComponent.generated.h"

class UJobDetail;
class UJobVisualDataAsset;
class USkeletalMeshComponent;

/*
* 유닛 액터에 붙이는 '직업'컴포넌트
* 에디터 디테일 패널에서  JobClass 드롭다운 선택하면
* BeginPlay에서 해당 직업로직(UObjectDetail 서브클래스) 자동생성
* 
* [사용법]
* BP_Player 에서 Add Component-> JobComponent 선택
* 디테일 패널에서 Job 카테고리 JobClass 드롭다운 에서 Warrior,Mage,Healer선택
* 플레이시 Beginplay에서 선택된 직업 로직 자동생성
* 
* [비주얼 자동 적용]
* JobVisual 맵에 EJobClass ->UJobVisualDataAsset 을연결해두면
* BeginPlay에 선택된 JobClass에 맞는 메시/AnimBp/몽타주세트를
* Owner의 SkeletalMeshComponent/ UUnitAnimComponent에 자동 주입한다.
* 
* [확장법]
* 새직업추가 EJobClass에 값을 추가하고 UJobDetail 서브클래스 작성
* SpawnDetail의 swtich에 케이스 추가
* 
* [연동]
* CombatManager / EffectManager에서 직업효과를 필요로할때
*  UJobComponent* Job = Unit->FindComponentByClass<UJobComponent>();
*  if (Job) FinalDamage = Job->ModifyCardDamage(CardTag, BaseDamage);
* 직업별 고유 자원이나 특수한 카드가 사용 될 때 처리로직을 처리
*/

UCLASS( ClassGroup=(Unit), meta=(BlueprintSpawnableComponent) )
class SLAYTHECHAMPIONS_API UJobComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UJobComponent();

protected:
	virtual void BeginPlay() override;

public:

	//에디터 설정
	/*
	* 이 유닛의 직업
	* 에디터 디테일 패널에서 Job카테고리에서 드롭다운으로 선택
	* Beginplay에서 이 값을 읽고 UJobDetail서브클래스 생성
	*/
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Job")
	EJobClass JobClass = EJobClass::Warrior;

	/*
	* 직업별 비주얼 매핑
	* EJobClass(드롭다운 값)->UJobVisualDataAsset(메시+AnimBP+몽타주)
	* 자동적용
	* 비어있거나 항목이 없으면 비주얼 주입건너뜀
	* 
	* [팁]파티원 BP들이 공유할 수 있도록 같은 DA 에셋들을 각 BP의 맵에 연결
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Job|Visual")
	TMap<EJobClass, TObjectPtr<UJobVisualDataAsset>> JobVisuals;

	/*
	* AnimData/AnimBP/메시를 주입할 SkeletalMeshComponent
	* null이면 Beginplay에서 Owner의 첫 번째 SkeletalMeshComponent를 자동으로 찾기
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Job|Visual")
	TObjectPtr<USkeletalMeshComponent> TargetMesh = nullptr;


	/*
	* 런타임에 직업을 바꾼다 — JobClass를 설정하고 Detail을 다시 생성.
	* CombatManager가 단일 BP_Player를 스폰한 뒤 챔피언별 직업을 주입할 때 사용.
	*/
	UFUNCTION(BlueprintCallable, Category = "Job")
	void SetJobClass(EJobClass NewJob);

	//직업 인터페이스 (외부 시스템 진입점)

	/*
	* 카드가 사용될 때 CardManager / CombatManager에서 호출
	* 내부적으로 UJobDetail::OnCardPlayered 에 위임
	*/
	UFUNCTION(BlueprintCallable, Category = "Job")
	void OnCardPlayed(FGameplayTag CardTag, int32 CardValue);

	/*
	* 데미지 계산시 Effectmanger에서 호출
	* 직업보정
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure,Category = "Job")
	float ModifyCardDamage(FGameplayTag CardTag, float BaseDamage);

	/** 턴 시작 시 CombatManager에서 호출한다. */
	UFUNCTION(BlueprintCallable, Category = "Job")
	void OnTurnStart();

	/** 턴 종료 시 CombatManager에서 호출한다. */
	UFUNCTION(BlueprintCallable, Category = "Job")
	void OnTurnEnd();

	/*
	* 현재 JobClass에 맞는 비주얼을 즉시 (재)적용
	*/
	UFUNCTION(BlueprintCallable, Category = "Job|Visual")
	void ApplyJobVisual();

	//직업 디테일

	/*
	* 현재 활성화된 UJobDetail반환
	* 직업별 세부 데이터(강화 스택수 등)에 접근할 때 Cast하여 사용
	* 
	* [예시]
	* UJobDetail_Warrior* Warrior =Cast<UJobDetail_Warrior>(Job->GetDetail());
	* if (Warrior) DrawCount += Warrior->EnhanceStack;
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Job")
	UJobDetail* GetDetail() const { return Detail; }




private:
	/** BeginPlay에서 JobClass에 맞는 UJobDetail 서브클래스를 생성한다. */
	void SpawnDetail();

	/** 생성된 직업 Detail 객체. nullptr이면 직업 없음(Any). */
	UPROPERTY()
	TObjectPtr<UJobDetail> Detail = nullptr;

};
