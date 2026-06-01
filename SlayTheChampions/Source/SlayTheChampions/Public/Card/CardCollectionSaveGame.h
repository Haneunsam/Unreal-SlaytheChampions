#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CardCollectionSaveGame.generated.h"

/**
 * UCardCollectionSaveGame
 *
 * 게임 전체에서 한 번이라도 획득한 카드 ID 목록을 영구 저장하는 클래스.
 * UCardSaveGame(덱 상태)과는 별개로 관리된다.
 *
 * [UCardSaveGame 과의 차이]
 * - UCardSaveGame        : 현재 덱 구성 (전투마다 바뀜)
 * - UCardCollectionSaveGame : 획득 이력 (한번 등록되면 삭제 불가)
 *
 * [사용 흐름]
 * 1. 카드 보상/구매 시 UnlockCard() 호출 -> 목록에 추가 후 즉시 저장
 * 2. 도감 열 때 IsUnlocked() 로 획득 여부 조회
 *
 * [슬롯 정보]
 * SlotName  : "CardCollectionSave"
 * UserIndex : 0
 */
UCLASS()
class SLAYTHECHAMPIONS_API UCardCollectionSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    // 획득한 카드 ID 목록 (DT_Cards RowName 기준)
    UPROPERTY(BlueprintReadWrite, Category = "Collection")
    TArray<FName> UnlockedCards;

    // 저장 슬롯 이름 (고정값)
    static const FString SlotName;

    // 저장 슬롯 유저 인덱스
    static const int32 UserIndex;

    // ── 정적 유틸 함수 ─────────────────────────────────────────────────────────

    /**
     * 디스크에서 SaveGame 을 로드한다.
     * 파일이 없으면 빈 SaveGame 을 새로 생성해서 반환 (절대 nullptr 반환 안 함).
     */
    UFUNCTION(BlueprintCallable, Category = "Collection")
    static UCardCollectionSaveGame* LoadOrCreate();

    /**
     * SaveGame 객체를 디스크에 저장한다.
     */
    UFUNCTION(BlueprintCallable, Category = "Collection")
    static void WriteSave(UCardCollectionSaveGame* SaveGame);

    /**
     * 카드를 획득 목록에 추가하고 즉시 저장한다.
     * 이미 등록된 카드면 중복 추가 안 함.
     *
     * @param CardID  DT_Cards RowName
     */
    UFUNCTION(BlueprintCallable, Category = "Collection")
    static void UnlockCard(FName CardID);

    /**
     * 카드가 획득 목록에 있는지 확인한다.
     *
     * @param CardID  DT_Cards RowName
     * @return 획득했으면 true
     */
    UFUNCTION(BlueprintPure, Category = "Collection")
    static bool IsUnlocked(FName CardID);

    /**
     * 획득한 카드 ID 목록 전체를 반환한다.
     */
    UFUNCTION(BlueprintPure, Category = "Collection")
    static TArray<FName> GetUnlockedCards();
};
