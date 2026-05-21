#include "CombatKernel/CombatantActor.h"
#include "CombatKernel/CombatStatComponent.h"
#include "CombatKernel/CombatStatWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"

ACombatantActor::ACombatantActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	StatComponent = CreateDefaultSubobject<UCombatStatComponent>(TEXT("StatComponent"));

	StatWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatWidgetComponent"));
	StatWidgetComponent->SetupAttachment(Mesh);
	StatWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	StatWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);   // 거리 무관 고정 크기
	StatWidgetComponent->SetDrawSize(FVector2D(200.f, 60.f));
}

void ACombatantActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// WidgetComponent::BeginPlay 이전에 클래스 세팅해야 위젯이 정상 생성됨
	if (StatWidgetClass)
		StatWidgetComponent->SetWidgetClass(StatWidgetClass);
}

void ACombatantActor::BeginPlay()
{
	Super::BeginPlay(); // 여기서 WidgetComponent::BeginPlay → 위젯 생성

	if (UCombatStatWidget* Widget = Cast<UCombatStatWidget>(StatWidgetComponent->GetUserWidgetObject()))
		Widget->InitWidget(StatComponent);
}
