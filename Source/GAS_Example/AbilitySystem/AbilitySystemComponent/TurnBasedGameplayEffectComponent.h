// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "TurnBasedGameplayEffectComponent.generated.h"

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
UCLASS()
class GAS_EXAMPLE_API UTurnBasedGameplayEffectComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()

public:
	/** 
	 * Number of turns until the effect activates (uninhibits). 
	 * Set to 0 for immediate activation on application.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn Settings", Meta = (ClampMin = 0))
	int32 InhibitionDelayTurns = 1;

	/** 
	 * Number of turns the effect stays active after activation (uninhibition). 
	 * Set to 0 to expire immediately after activation.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn Settings", Meta = (ClampMin = 0))
	int32 DurationTurns = 1;

	/** Optional: GE applied when this effect activates (on uninhibition).*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn Actions")
	TSubclassOf<UGameplayEffect> EffectToApplyOnUninhibition;

	/** Optional: GE applied when this effect expires (on removal).*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn Actions")
	TSubclassOf<UGameplayEffect> EffectToApplyOnRemoval;

	/** If enabled, applies the Periodic GE every turn while active (post-activation).*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Periodic Settings")
	bool bEnablePeriodicGE = false;

	/** 
	 * Optional: GE applied at the start of each turn while this effect is active. 
	 * Only used if "Enable Periodic GE" is enabled.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Periodic Settings", Meta = (EditCondition = "bEnablePeriodicGE == true", EditConditionHides))
	TSubclassOf<UGameplayEffect> EffectToApplyPeriodically;

	/** Limits how many times the periodic GE can trigger.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Periodic Settings", meta = (EditCondition = "bEnablePeriodicGE == true", EditConditionHides))
	bool bLimitPeriodicApplications = false;

	/** 
	 * Maximum number of times the periodic GE will be applied. 
	 * Only used if "Limit Periodic Applications" is enabled.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Periodic Settings",
		Meta = (EditCondition = "bEnablePeriodicGE == true && bLimitPeriodicApplications == true", EditConditionHides), Meta = (ClampMin = 1))
	int32 MaxPeriodicApplications = 1;
	
	UTurnBasedGameplayEffectComponent();

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

	virtual bool CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec) const override;
	virtual bool OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer, FActiveGameplayEffect& ActiveGE) const override;
	
protected:
	void OnTurnChange(int32 NewTurn, int32 StartTurn, FActiveGameplayEffectHandle Handle) const;
	void OnGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, UAbilitySystemComponent* ASC, FDelegateHandle DelegateHandle) const;

private:
	static ITurnSystemInterface* GetTurnSystem(const UAbilitySystemComponent* ASC);
	static void ApplyGameEffect(UAbilitySystemComponent* ASC, const TSubclassOf<UGameplayEffect>& GameEffectToApply);
	void ProcessPeriodicEffect(UAbilitySystemComponent* OwnerASC, int ActiveTurns) const;	
};
