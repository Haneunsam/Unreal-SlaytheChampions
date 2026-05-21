#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CardDataTypes.h"
#include "HandComponentTest.generated.h"
/**
 * AHandComponentTester
 *
 * 에디터 PIE(Play In Editor) 에서 Pawn1 / Pawn2 의
 * HandComponent 동작을 로그로 검증하는 테스트 Actor.
 *
 * 사용법:
 *  1. 레벨에 이 Actor 를 배치한다.
 *  2. 디테일 패널에서 Pawn1Ref / Pawn2Ref 에 레벨의 Pawn 을 할당한다.
 *  3. PIE 실행 → Output Log 에서 [HandTest] 태그로 결과 확인.
 */
UCLASS()
class SLAYTHECHAMPIONS_API AHandComponentTester : public AActor
{
    GENERATED_BODY()

public:
    AHandComponentTester();

protected:
    virtual void BeginPlay() override;

public:
    // ── 테스트 대상 ───────────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, Category = "HandTest")
    TObjectPtr<APawn> Pawn1Ref;

    UPROPERTY(EditAnywhere, Category = "HandTest")
    TObjectPtr<APawn> Pawn2Ref;

    // ── 테스트 실행 (Blueprint 에서도 수동 호출 가능) ──────────────────────
    UFUNCTION(BlueprintCallable, Category = "HandTest")
    void RunAllTests();

private:
    void TestPawn(APawn* Pawn, const FString& Label);

    // 개별 테스트 케이스
    void Test_InitialState(class UHandComponent* HC, const FString& Label);
    void Test_DrawStartOfTurn(class UHandComponent* HC, const FString& Label);
    void Test_PlayCard(class UHandComponent* HC, const FString& Label);
    void Test_DiscardHand(class UHandComponent* HC, const FString& Label);
    void Test_RecycleDiscard(class UHandComponent* HC, const FString& Label);

    // 로그 헬퍼
    static void Pass(const FString& Label, const FString& Msg);
    static void Fail(const FString& Label, const FString& Msg);
    static void Info(const FString& Label, const FString& Msg);
};