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
    bool GetCachedRelicData(FName InRelicID, FRelic& OutRelicData) const;

    UFUNCTION(BlueprintCallable, Category = "Relic|Query")
    TArray<FRelic> GetCachedRelics() const;

private:
    void RebuildRelicCache();

    static FRelicEffectData MakeRelicEffectData(const FRelicEffectRow& EffectRow);

    static void FillRelicRuntimeData(const FRelicDataRow& RelicRow, TArray<FRelicEffectData>&& Effects, FRelic& OutRelicData);

    UPROPERTY()
    TObjectPtr<UDataTable> RelicInfoTable;

    UPROPERTY()
    TObjectPtr<UDataTable> RelicEffectsTable;

    UPROPERTY()
    TArray<FRelic> Relics;

    TMap<FName, FRelic> Map_Relics;
};
