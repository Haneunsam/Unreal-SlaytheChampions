// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Unit/UnitAnimDataAsset.h"
#include "JobVisualDataAsset.generated.h"

class USkeletalMesh;
class UAnimInstance;
/**
 * UJobVisualDataAsset
 * 직업 한 종류(검사/마법사/힐러) 의 비주얼 전체를 담는 DataAsset
 * 스켈레탈 메시 + AnimBP + 메시 트랜스폼 + 몽타주세트(UUnitAnimDataAsset)를 한곳에 담기
 * 
 * 직업별 데이터 에셋 생성후 지정후
 * UJobComponent의 JobVisuals 맵에 EJobClass DA를 연결
 * (BeginPlay 에서 선택된 DA에 자동적용)
 */
UCLASS(BlueprintType)
class SLAYTHECHAMPIONS_API UJobVisualDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	//메시
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Visual|Mesh")
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual|Mesh")
	TSubclassOf<UAnimInstance> AnimBPClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Visual|Mesh")
	FVector MeshScale = FVector(1.f);

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Visual|Mesh")
	FVector MeshOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual|Mesh")
	FRotator MeshRotation = FRotator::ZeroRotator;

	//몽타주세트
	// UUnitAnimComponent::AnimData 로 주입되며 Attack/Hit/Death/Skill 등에 사용된다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Visual|Animation")
	TObjectPtr<UUnitAnimDataAsset> AnimData;
};
