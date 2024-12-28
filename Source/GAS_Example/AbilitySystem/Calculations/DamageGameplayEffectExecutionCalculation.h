// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "DamageStatics.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GAS_Example/AbilitySystem/AbilitySystemComponent/CustomAbilitySystemComponent.h"
#include "DamageGameplayEffectExecutionCalculation.generated.h"

/**
 * Damage calculation. Should be used to apply damage to anyone.
 */
UCLASS()
class GAS_EXAMPLE_API UDamageGameplayEffectExecutionCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

protected:
	static const FCustomDamageStatics* DamageStatics;
	
public:

	UDamageGameplayEffectExecutionCalculation();
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	/**
	 * Convenience Method to calculate status damage. Can be reused for any new status.
	 * @param TargetASC Target Ability System Component
	 * @param Spec
	 * @param DamageSign
	 * @param OutExecutionOutput
	 * @param ExecutionParams 
	 * @param EvaluationParameters 
	 * @param AttributeDef Status damage received attribute
	 * @param OutputProperty Status to update
	 * @param DamageType
	 * @param ResistanceTag
	 * @param IgnoreResistanceTag 
	 * @param ImmunityTag Immunity tag, if any (if invalid -> no immunity). Only checks GE's asset tags (for now).
	 * @param DamageMultiplier Any damage multiplier you want to apply. Could be armor, or anything else.
	 */
	static void CalculateDamageAndApply(
		const UCustomAbilitySystemComponent& TargetASC,
		const FGameplayEffectSpec& Spec,
		float DamageSign,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		const FAggregatorEvaluateParameters& EvaluationParameters,
		const FGameplayEffectAttributeCaptureDefinition& AttributeDef,
		FProperty* OutputProperty,
		const FGameplayTag& DamageType = FGameplayTag::EmptyTag,
		const FGameplayTag& ResistanceTag = FGameplayTag::EmptyTag,
		const FGameplayTag& IgnoreResistanceTag = FGameplayTag::EmptyTag,
		const FGameplayTag& ImmunityTag = FGameplayTag::EmptyTag
	);
};
