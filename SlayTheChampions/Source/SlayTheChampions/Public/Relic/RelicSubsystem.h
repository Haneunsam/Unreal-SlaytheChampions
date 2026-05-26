#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Relic/RelicStruct.h"
#include "RelicSubsystem.generated.h"

class UDataTable;

UCLASS()
class SLAYTHECHAMPIONS_API URelicSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Relic|Query")
    bool GetCachedRelicData(FName InRelicID, FRelicRuntimeData& OutRelicData) const;

    UFUNCTION(BlueprintCallable, Category = "Relic|Query")
    TArray<FRelicRuntimeData> GetCachedRelics() const;

private:
    void RebuildRelicCache();

    static FRelicEffectData MakeRelicEffectData(const FRelicEffectRow& EffectRow);

    static void FillRelicRuntimeData(const FRelicDataRow& RelicRow, TArray<FRelicEffectData>&& Effects, FRelicRuntimeData& OutRelicData);

    UPROPERTY()
    TObjectPtr<UDataTable> RelicInfoTable;

    UPROPERTY()
    TObjectPtr<UDataTable> RelicEffectsTable;

    UPROPERTY()
    TArray<FRelicRuntimeData> Relics;

    TMap<FName, FRelicRuntimeData> Map_Relics;
};
