// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "SimpleDamageStatics.h"
#include "GAS_Example/AbilitySystem/AbilitySystemComponent/CustomAbilitySystemComponent.h"
#include "SimpleDamageGameplayEffectExecutionCalculation.generated.h"

/**
 * Simple Damage calculation. Should be used to apply damage to anyone.
 */
UCLASS()
class GAS_EXAMPLE_API USimpleDamageGameplayEffectExecutionCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

protected:
	static const FSimpleCustomDamageStatics* DamageStatics;
	
public:

	USimpleDamageGameplayEffectExecutionCalculation();
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
