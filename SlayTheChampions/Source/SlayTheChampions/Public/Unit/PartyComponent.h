// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Unit/CombatTypes.h" //ETeam과 ETargetType(CardDataType)사용
#include "PartyComponent.generated.h"

class AUnit;
class UStatComponent;


/*
* UPartyComponent
* 한팀에 속한 유닛 <묶음>을 관리하는 재사용 컴포넌트
* 전투규칙은 들어가지 않고, 누가 파티에 있고,누가 선택됐고, 누가 살아있는가만 책임진다.
* 외부와는 델리게이트 통신
* 
* [인덱스 안정성]
* 멤버가 죽어도 배열에서 제거하지 않는다.(슬롯위치고정)
* CombatManager의 CasterIndex와 같은 "인덱스키"가 전투도중 어긋나지 않게
* 살아있는 멤버만 필요하면 GetAliveMembers() 사용
*/

UCLASS( ClassGroup=(Unit), meta=(BlueprintSpawnableComponent) )
class SLAYTHECHAMPIONS_API UPartyComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPartyComponent();

	//이 파티의 소속팀 Ally(아군파티),Enemy(적그룹)구분에 사용
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Party")
	ETeam PartyTeam = ETeam::Ally;

	//파티의 최대 인원(3명)
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Party",meta = (ClampMin = "1",ClampMax="3"))
	int32 MaxMembers = 3;

	//Delegate
	//멤버등록
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPartyMemberRegistered, AUnit*, Unit, int32, Index);
	UPROPERTY(BlueprintAssignable, Category = "Party")
	FOnPartyMemberRegistered OnMemberRegistered;
	//멤버사망
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPartyMemberDied, AUnit*, Unit, int32, Index);
	UPROPERTY(BlueprintAssignable, Category = "Party")
	FOnPartyMemberDied OnMemberDied;
	//슬롯바뀜
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPartySelectionChanged, int32, OldIndex, 
		int32, NewIndex, AUnit*, NewUnit);
	UPROPERTY(BlueprintAssignable, Category = "Party")
	FOnPartySelectionChanged OnSelectionChanged;

	//파티전멸
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPartyWiped);
	UPROPERTY(BlueprintAssignable, Category = "Party")
	FOnPartyWiped OnPartyWiped;


	//턴이벤트 델리게이트
	//BroadcastTurnStart/End가 살아있는 멤버 수만큼 반복호출
	//PartyComponent는 누구에게 전달할지만 알고 무엇을 할지는 모른다.
	//바인딩쪽이 책임진다.
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPartyTurnEvent, AUnit*, Member);

	// 턴 시작 시 생존 멤버 한 명씩 브로드캐스트.
	UPROPERTY(BlueprintAssignable, Category = "Party|Turn")
	FOnPartyTurnEvent OnTurnStartPerMember;

	// 턴 종료 시 생존 멤버 한 명씩 브로드캐스트.
	UPROPERTY(BlueprintAssignable, Category = "Party|Turn")
	FOnPartyTurnEvent OnTurnEndPerMember;


	//로스터 관리
	/*
	* 파티에 유닛을 등록한다.(CombatManager 가 유닛 스폰 직후 호출하는 진입점)
	* OnUnitDied 에 내부 핸들러 바인딩
	* 파티가 가득찼거나 유닛이 유효하지 않는다면 INDEX_NONE 반환
	* @return 부여된 슬롯 인덱스(실패시 INDEX_NONE)
	*/

	UFUNCTION(BlueprintCallable, Category = "Party")
	int32 RegisterMember(AUnit* Unit);


	//멤버 완전삭제(슬롯도 비움).일반 전투중엔 거의 쓰지 않음
	UFUNCTION(BlueprintCallable, Category = "Party")
	void UnregisterMember(AUnit* Unit);

	//전체 비우기
	UFUNCTION(BlueprintCallable, Category = "Party")
	void ClearParty();

	//로스터 조회

	//슬롯 인덱스로 멤버반환(사망멤버포함, 범위밖이면 nullptr)
	UFUNCTION(BlueprintPure, Category = "Party")
	AUnit* GetMember(int32 Index)const;

	// 유닛의 슬롯 인덱스 반환(없으면 INDEX_NONE)
	UFUNCTION(BlueprintPure, Category = "Party")
	int32 GetMemberIndex(const AUnit* Unit) const;

	//등록된 전체 멤버수
	UFUNCTION(BlueprintPure,Category = "Party")
	int32 Num() const { return Members.Num(); }

	//전체 멤버 배열(사망포함)
	UFUNCTION(BlueprintPure, Category = "Party")
	TArray<AUnit*> GetAllMembers() const;

	//살아있는 멤버만 반환
	UFUNCTION(BlueprintPure, Category = "Party")
	TArray<AUnit*> GetAliveMembers() const;

	//살아있는 멤버 수
	UFUNCTION(BlueprintPure, Category = "Party")
	int32 GetAliveCount() const;

	//등록된 멤버가 있으나 전원사망이면 True(빈파티면 false)
	UFUNCTION(BlueprintPure,Category = "Party")
	bool IsWiped() const;


	//선택(주로 아군파티 UI용)
	//인덱스로 선택. 죽었거나 버뮈 밖이면 false
	UFUNCTION(BlueprintCallable, Category = "Party")
	bool SelectMember(int32 Index);

	//유닛으로 선택
	UFUNCTION(BlueprintCallable, Category = "Party")
	bool SelectUnit(AUnit* Unit);

	UFUNCTION(BlueprintPure, Category = "Party")
	int32 GetSelectedIndex() const { return SelectedIndex; }

	UFUNCTION(BlueprintPure, Category = "Party")
	AUnit* GetSelectedMemer() const;

	//타겟 선정 프리미티브 (적AI/ 힐러 등에서 조합해서 사용)
	//주의: 이 함수들은 "파티 내부"만 본다. 적<->아군 교차 타겟팅은
	//		전투/타겟팅 레이어가 두 파티 컴포넌트를 조합하여 해석

	//살아있는 멤버중 무작위 한명(없으면 nullptr)
	UFUNCTION(BlueprintPure, Category = "Party|Targeting")
	AUnit* GetRandomAliveMember() const;

	//HP비율이 가장 낮은 살아있는 멤버
	UFUNCTION(BlueprintPure, Category = "Party|Targeting")
	AUnit* GetLowestHPMember() const;

	//턴진행
	UFUNCTION(BlueprintCallable, Category = "Party|Turn")
	void BroadcastTurnStart();

	UFUNCTION(BlueprintCallable, Category = "Party|Turn")
	void BroadcastTurnEnd();



protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	


private:
	//파티슬롯. 죽어도 제거하지 않아 인덱스가 고정
	UPROPERTY()
	TArray<TObjectPtr<AUnit>> Members;
		
	//현재 선택된 슬롯(없으면 INDEX_NONE)
	int32 SelectedIndex = INDEX_NONE;

	//AUnit::OnUnitDied 에 바인딩되는 내부 핸들러
	UFUNCTION()
	void HandleMemberDied(AUnit* Unit);

	//멤버의 OnUnitDied 바인드/언바인드 헬퍼
	void BindMember(AUnit* Unit);
	void UnbindMember(AUnit* Unit);
};
