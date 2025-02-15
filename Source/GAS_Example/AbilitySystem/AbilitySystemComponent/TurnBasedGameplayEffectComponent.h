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
	/** The number of turns until uninhibition of the Turn-Based GE. When set to 0 the GE is uninhibited instantly*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn Settings", Meta = (ClampMin = 0))
	int32 InhibitionDelayTurns = 1;

	/** The number of turns, after uninhibition of the Turn-Based GE, until it is removed. When set to 0 the GE is removed immediately after inhibition*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn Settings", Meta = (ClampMin = 0))
	int32 DurationTurns = 1;

	/** Optional: GE to apply on uninhibition of the Turn-Based GE*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn Actions")
	TSubclassOf<UGameplayEffect> GEToApplyOnUninhibition;

	/** Optional: GE to apply on removal of the Turn-Based GE*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn Actions")
	TSubclassOf<UGameplayEffect> GEToApplyOnRemoval;

	/** If enabled, applies the Periodic GE each turn, starting when the Turn-Based GE becomes uninhibited for DurationTurns*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Periodic Settings")
	bool bEnablePeriodicGE = false;

	/** Optional: Periodic GE to apply at the start of each uninhibited turn*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Periodic Settings", Meta = (EditCondition = "bEnablePeriodicGE == true", EditConditionHides))
	TSubclassOf<UGameplayEffect> GEToApplyPeriodically;

	/** If enabled, limits how many times the periodic effect can be applied*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Periodic Settings", meta = (EditCondition = "bEnablePeriodicGE == true", EditConditionHides))
	bool bLimitPeriodicApplications = false;

	/** The maximum times to apply the Periodic GE throughout the duration of the Turn-Based GE*/
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
