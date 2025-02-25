// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GAS_Example/AbilitySystem/GameplayEffects/CustomGameplayEffect.h"
#include "TurnBasedGameplayEffect.generated.h"

/**
 * Simplifies the creation of Turn Based Gameplay Effects for designers.  This class automatically sets the GE duration
 * to infinite and adds/manages the required `TurnBasedGameplayEffectComponent`.
 */
UCLASS()
class GAS_EXAMPLE_API UTurnBasedGameplayEffect : public UCustomGameplayEffect
{
	GENERATED_BODY()

public:
	UTurnBasedGameplayEffect();

	virtual void PostInitProperties() override;
	
	/** Returns an effect context, given a specified actor info */
	UFUNCTION(BlueprintPure)
	static FGameplayEffectContextHandle MakeTurnBasedEffectContext(
		float Level,
		UAbilitySystemComponent* ASC,
		const UGameplayAbility* InGameplayAbility,
		FGameplayAbilitySpecHandle Handle,
		FGameplayAbilityActorInfo ActorInfo,
		const TSubclassOf<UTurnBasedGameplayEffect>& TurnBasedEffectClass);

	/**
	 * Update the current Effect context to derive TurnBase informations (Duration, Inhibition turns etc ...)
	 * This is mandatory for any Turnbased Effect.
	 * @param EffectContextHandle Current Context effect handle to update
	 * @param Level Level to use
	 * @param TurnBasedEffectClass Turn-based effect class from we'll extract the base duration etc ... (see TurnBasedEffectComponent) 
	 * @param ASC Source ASC
	 * @param SourceAggregatedTags Source Aggregated tags, from the ASC Owned Tags + GE Asset Tags + [Current Ability Asset Tags if any]
	 * @return true if the effect context was updated to the TurnBased requirement, false if not.
	 */
	UFUNCTION(BlueprintPure)
	static bool UpdateTurnBasedEffectContext(
		FGameplayEffectContextHandle& EffectContextHandle,
		float Level,
		const TSubclassOf<UTurnBasedGameplayEffect>& TurnBasedEffectClass,
		UAbilitySystemComponent* ASC,
		const FGameplayTagContainer& SourceAggregatedTags);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR
	
};
