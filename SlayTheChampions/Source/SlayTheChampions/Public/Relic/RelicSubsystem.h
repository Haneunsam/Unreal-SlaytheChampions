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

	UFUNCTION(BlueprintCallable, Category = "Relic|Data")
	void LoadRelicDataTable(UDataTable* InTable);

	UFUNCTION(BlueprintCallable, Category = "Relic|Data")
	void LoadRelicEffectDataTable(UDataTable* InTable);

	UFUNCTION(BlueprintCallable, Category = "Relic|Query")
	bool GetRelicRuntimeData(FName InRelicID, FRelicRuntimeData& OutRelicData) const;

	UFUNCTION(BlueprintCallable, Category = "Relic|Query")
	TArray<FRelicEffectData> GetRelicEffectData(FName InRelicID) const;

	UFUNCTION(BlueprintCallable, Category = "Relic|Query")
	TArray<FName> GetAllRelicIDs() const;

	const FRelicDataRow* GetRelicDataRow(FName InRelicID) const;

private:
	UPROPERTY()
	TObjectPtr<UDataTable> RelicDataTable;

	UPROPERTY()
	TObjectPtr<UDataTable> RelicEffectDataTable;
};
