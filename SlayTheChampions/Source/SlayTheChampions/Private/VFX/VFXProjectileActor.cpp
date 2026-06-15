// Fill out your copyright notice in the Description page of Project Settings.


#include "VFX/VFXProjectileActor.h"
#include "Unit/Unit.h"
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AVFXProjectileActor::AVFXProjectileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false; // Launch() 전까지 비활성
	// 루트 씬 컴포넌트
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// 파티클 컴포넌트 (Launch 전엔 비활성)
	ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
	ParticleComp->SetupAttachment(Root);
	ParticleComp->bAutoActivate = false;

}

// Called when the game starts or when spawned
void AVFXProjectileActor::BeginPlay()
{
	Super::BeginPlay();

}


void AVFXProjectileActor::Launch(UParticleSystem* InParticle, AUnit* InTarget, float InSpeed,
								UParticleSystem* InImpactParticle, float InImpactDuration, bool bInImpactImmediateStop)
{
	if (!InParticle || !InTarget)
	{
		Destroy();
		return;
	}

	TargetUnit = InTarget;
	MoveSpeed = FMath::Max(100.f, InSpeed);
	ImpactParticle = InImpactParticle;
	ImpactDuration = InImpactDuration;
	bImpactImmediateStop = bInImpactImmediateStop;

	// 파티클 설정 및 활성화
	ParticleComp->SetTemplate(InParticle);
	ParticleComp->Activate(true);

	bLaunched = true;
	PrimaryActorTick.SetTickFunctionEnable(true);

	UE_LOG(LogTemp, Log,
		TEXT("[VfxProjectile] '%s' Launch -> '%s' (Speed: %.0f)"),
		*InParticle->GetName(),
		*InTarget->GetName(),
		MoveSpeed);
}


// Called every frame
void AVFXProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bLaunched) return;

	if (!TargetUnit.IsValid())
	{
		Destroy();
		return;
	}

	const FVector CurrentLoc = GetActorLocation();
	const FVector TargetLoc = TargetUnit->GetActorLocation();
	const float   Distance = FVector::Dist(CurrentLoc, TargetLoc);

	if (Distance <= ArrivalThreshold)
	{
		OnArrived();
		return;
	}

	const FVector Dir = (TargetLoc - CurrentLoc).GetSafeNormal();
	const FVector NewLoc = CurrentLoc + Dir * MoveSpeed * DeltaTime;
	SetActorLocation(NewLoc);

	if (!Dir.IsNearlyZero())
		SetActorRotation(Dir.Rotation());
}

void AVFXProjectileActor::BeforeImpact()
{
	// 발사체 파티클 비활성화 (도달 시 Trail 이펙트 정지)
	if (ParticleComp)
		ParticleComp->Deactivate();

	UE_LOG(LogTemp, Log,
		TEXT("[VfxProjectile] '%s' BeforeImpact"), *GetName());
}




void AVFXProjectileActor::OnArrived()
{
	//도착 처리는 한번만 Tick이 들어와도 재진입 차단
	if (!bLaunched) return;
	bLaunched = false;
	PrimaryActorTick.SetTickFunctionEnable(false);

	//발사체 트레일 정지
	BeforeImpact();
	if (ImpactParticle && TargetUnit.IsValid())
	{
		UParticleSystemComponent* ImpactPsc = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), ImpactParticle,
			TargetUnit->GetActorLocation(),
			FRotator::ZeroRotator, true);

		// Duration이 있으면 루프형 충돌 이펙트도 강제 종료
		// (이 액터는 곧 Destroy되지만 임팩트 PSC와 월드 타이머는 독립적이라 안전)
		if (ImpactPsc && ImpactDuration > 0.f)
		{
			if (UWorld* World = GetWorld())
			{
				TWeakObjectPtr<UParticleSystemComponent> WeakPsc(ImpactPsc);
				const bool bImmediate = bImpactImmediateStop;
				FTimerHandle Handle;
				World->GetTimerManager().SetTimer(Handle,
					[WeakPsc, bImmediate]()
					{
						UParticleSystemComponent* Comp = WeakPsc.Get();
						if (!Comp) return;
						Comp->bAutoDestroy = true;
						if (bImmediate) Comp->DeactivateImmediate();
						else            Comp->Deactivate();
					},
					ImpactDuration, false);
			}
		}
	}
	Destroy();
}

