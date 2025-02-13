// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "UObject/Object.h"
#include "TargetFinderBase.generated.h"

UENUM()
enum class ETargetFinder_Pins : uint8
{
	TargetsFound,
	NoTargets
};

UCLASS(Blueprintable, BlueprintType, Abstract)
class CUSTOMROGUELITETOOLS_API UTargetFinderBase : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagRequirements TagRequirements;

	UFUNCTION(BlueprintPure)
	bool CheckTagRequirementsForTarget(const AActor* Target);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TraceTargets(const AActor* SourceActor, TArray<AActor*>& OutActors, TArray<FHitResult>& OutHitResults);

	UFUNCTION(BlueprintCallable)
	bool IsTargetValid(const AActor* SourceActor, const AActor* TargetActor);
	
	UFUNCTION(BlueprintNativeEvent)
	bool K0_IsTargetValid(const AActor* SourceActor, const AActor* TargetActor);

public:
	UFUNCTION(BlueprintCallable, meta=(ExpandEnumAsExecs="Pins"))
	void FindTargets(const AActor* SourceActor, TArray<AActor*>& OutActors, TArray<FHitResult>& OutHitResults, ETargetFinder_Pins& Pins);

	UFUNCTION(BlueprintNativeEvent)
	void K0_FindTargets(const AActor* SourceActor, TArray<AActor*>& OutActors, TArray<FHitResult>& OutHitResults);

	virtual bool ImplementsGetWorld() const override { return true;};

	virtual class UWorld* GetWorld() const override;
};
