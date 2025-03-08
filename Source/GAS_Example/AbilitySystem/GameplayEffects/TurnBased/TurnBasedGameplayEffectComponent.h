// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "ScalableFloat.h"
#include "TurnBasedGameplayEffect.h"
#include "GAS_Example/AbilitySystem/Data/AbilitySystemData.h"
#include "TurnBasedGameplayEffectComponent.generated.h"


// Contains Instanced turn Data
USTRUCT(BlueprintType)
struct FCustomContextData_TurnBaseEffect: public FCustomContextDataBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 StartTurn = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 InhibitionDelayTurns = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 DurationTurns = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 MaxPeriodicApplications = 0;
	
	FCustomContextData_TurnBaseEffect() {}

	FCustomContextData_TurnBaseEffect(const int32 InStartTurn, const int32 InInhibititionDelayTurns, const int32 InDurationTurns, const int32 InMaxPeriodicApplications)
	{
		StartTurn = FMath::Max(InStartTurn, 0);
		InhibitionDelayTurns = FMath::Max(InInhibititionDelayTurns, 0);
		DurationTurns = FMath::Max(InDurationTurns, 0);
		MaxPeriodicApplications = FMath::Max(InMaxPeriodicApplications, 1); 
	}

	bool IsValid() const;
	
	/**
	 * Get the absolute start turn and the end turn from this definition
	 * @return Interval where End - Start = Duration
	 */
	FInt32Interval GetActivationBounds() const;

	/**
	 * Get the absolute inhibition start turn and inhibition end turn from this definition
	 * @return Interval where End - Start = Inhibition Delay
	 */
	FInt32Interval GetInhibitionBounds() const;

	/**
	 * Return if the effect was inhibited on the last turn, but is not anymore.
	 * @param CurrentTurn Current Turn
	 * @return true if was inhibited, false if not.
	 */
	bool WasInhibited(int32 CurrentTurn) const;
	
	/**
	 * Return how many inhibition turns are remaining, based on the current turn.
	 * @param CurrentTurn Turn you want to extrapolate the value for
	 * @return How much turn this effect should be inhibited (0+). 0 = No inhibition
	 */
	int32 GetRemainingInhibitionTurn(const int32 CurrentTurn) const;

	// Return the first turn where this effect will be activated
	int32 GetActivationStartTurn() const;

	// Return the last turn where this effect will be activated
	int32 GetActivationEndTurn() const;
	/**
	 * Return how many turns are remaining, based on the current turn.
	 * @param CurrentTurn Turn you want to extrapolate the value for
	 * @return How much turn this effect should be active for (0+). 0 = Not active anymore. Return Duration turns if inhibited
	 */
	int32 GetRemainingActivationTurn(const int32 CurrentTurn) const;

	/**
	 * Check if this effect should be inhibited or not.
	 * @param CurrentTurn Current turn you want to know
	 * @return true if inhibited (or deleted), false if it should be active
	 */
	bool ShouldBeInhibited(const int32 CurrentTurn) const;
	
	/**
	 * Calculate, base on the Current turn, if this effect should still be there, or not.
	 * @param CurrentTurn 
	 * @return true if this effect should be removed.
	 */
	bool ShouldBeRemoved(const int32 CurrentTurn) const;

	/** Overridden to serialize new fields */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
};

template <>
struct TStructOpsTypeTraits<FCustomContextData_TurnBaseEffect> : public TStructOpsTypeTraitsBase2<FCustomContextData_TurnBaseEffect>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true // Necessary so that TSharedPtr<STRUCT> Data is copied around
	};
};

class ITurnSystemInterface;
class UTurnWorldSubsystem;
class UAbilitySystemComponent;
struct FGameplayEffectRemovalInfo;
struct FActiveGameplayEffectHandle;

DECLARE_LOG_CATEGORY_EXTERN(LogTurnSystem, Log, All);

/**
 * Manages the application of a Turn-Based Gameplay Effect (GE), along with the component's additional GEs, within a turn-based system.
 * This class listens for turn changes via the `TurnWorldSubsystem` and determines when to uninhibit and remove the
 * Turn-Based GE, as well as when to apply the component's additional GEs.
 */
UCLASS(BlueprintType)
class GAS_EXAMPLE_API UTurnBasedGameplayEffectComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()

public:
	/** 
	 * Number of turns until the effect activates (uninhibits). 
	 * Set to 0 for immediate activation on application.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn Settings", Meta = (ClampMin = 0))
	FScalableFloat InhibitionDelayTurns = 1;

	/** 
	 * Number of turns the effect stays active after activation (uninhibition). 
	 * Set to 0 to expire immediately after activation.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn Settings", Meta = (ClampMin = 0))
	FScalableFloat DurationTurns = 1;

	/** Optional: GE applied when this effect activates (on uninhibition).*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn Actions")
	TSet<TSubclassOf<UGameplayEffect>> EffectToApplyOnUninhibition;

	/** Optional: GE applied when this effect expires (on removal).*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn Actions")
	TSet<TSubclassOf<UGameplayEffect>> EffectToApplyOnRemoval;

	/** If enabled, applies the Periodic GE every turn while active (post-activation).*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Periodic Settings")
	bool bEnablePeriodicGE = false;

	/** 
	 * Optional: GE applied at the start of each turn while this effect is active. 
	 * Only used if "Enable Periodic GE" is enabled.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Periodic Settings", Meta = (EditCondition = "bEnablePeriodicGE == true", EditConditionHides))
	TSet<TSubclassOf<UGameplayEffect>> EffectToApplyPeriodically;

	/** Limits how many times the periodic GE can trigger.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Periodic Settings", meta = (EditCondition = "bEnablePeriodicGE == true", EditConditionHides))
	bool bLimitPeriodicApplications = false;

	/** 
	 * Maximum number of times the periodic GE will be applied. 
	 * Only used if "Limit Periodic Applications" is enabled.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Periodic Settings", Meta = (EditCondition = "bEnablePeriodicGE == true && bLimitPeriodicApplications == true", EditConditionHides), Meta = (ClampMin = 1))
	FScalableFloat MaxPeriodicApplications = 1;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay)
	bool bCopyContextFromSource = true;
	
	UTurnBasedGameplayEffectComponent();

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

	virtual bool CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec) const override;
	virtual bool OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer, FActiveGameplayEffect& ActiveGE) const override;
	
	void OnTurnChange(const int32 NewTurn, FActiveGameplayEffectHandle Handle) const;
	void OnGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, UAbilitySystemComponent* ASC, FDelegateHandle DelegateHandle) const;

protected:
	void ProcessPeriodicEffect(UAbilitySystemComponent* OwnerASC, int32 ActiveTurns, FActiveGameplayEffectHandle Handle) const;	
};
