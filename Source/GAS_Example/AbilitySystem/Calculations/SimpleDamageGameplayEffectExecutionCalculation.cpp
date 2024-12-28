// Copyright 2021 Joseph "Narxim" Thigpen.


#include "SimpleDamageGameplayEffectExecutionCalculation.h"
#include "GAS_Example/AbilitySystem/AbilitySystemComponent/CustomAbilitySystemComponent.h"
#include "GAS_Example/AbilitySystem/FunctionLibrary/CustomAbilitySystemBlueprintLibrary.h"
#include "GAS_Example/Characters/CharacterBase.h"

const FSimpleCustomDamageStatics* USimpleDamageGameplayEffectExecutionCalculation::DamageStatics = nullptr;

USimpleDamageGameplayEffectExecutionCalculation::USimpleDamageGameplayEffectExecutionCalculation()
{
	if (!DamageStatics)
	{
		DamageStatics = &FSimpleCustomDamageStatics::DamageStatics();
	}
	
	RelevantAttributesToCapture.Add(DamageStatics->ResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics->CurrentHealthDef);
	RelevantAttributesToCapture.Add(DamageStatics->MaximumHealthDef);
	RelevantAttributesToCapture.Add(DamageStatics->DamageDef);
}

void USimpleDamageGameplayEffectExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	if (!DamageStatics)
	{
		DamageStatics = &FSimpleCustomDamageStatics::DamageStatics();
	}

	if (!DamageStatics)
	{
		return;
	}
	
	UCustomAbilitySystemComponent* const TargetAbilitySystemComponent = Cast<UCustomAbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());

	if (!TargetAbilitySystemComponent)
	{
		return;
	}

	// Feel free to uncomment any of those lines to have access to other objects.
	// UCustomAbilitySystemComponent* const SourceAbilitySystemComponent = Cast<UCustomAbilitySystemComponent>(ExecutionParams.GetSourceAbilitySystemComponent());

	// AActor* const SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	// AActor* const TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;

	// ACharacterBase* const SourceCharacter = Cast<ACharacterBase>(SourceActor);
	// ACharacterBase* const TargetCharacter = Cast<ACharacterBase>(TargetActor);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FAggregatorEvaluateParameters EvaluationParameters{};
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float DamageRaw = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics->DamageDef, EvaluationParameters, DamageRaw);
	DamageRaw = FMath::Max(DamageRaw, 0.f);

	// No Damage, we can return.
	if (DamageRaw == 0.f)
	{
		return;
	}

	// Calculate the resistance attribute.
	
	/* We are using LoL Resistance logic
	 * https://www.quora.com/How-does-Resistance-work-in-League-of-Legends
	 * Resistance provides diminishing returns; as a champion’s Resistance increases, the percentage of damage reduced from physical attacks becomes less effective.
	 * This means that stacking Resistance is beneficial, but there are limits to its effectiveness.
	 * Note:	Removing Resistance will scale very fast (-50 Resistance = x 2 damage)
	 *			Could be clamped in the HealthAttributeSet to never go below 0.
	 *
	 * We are using KaosSpectrum implementation of https://www.thegames.dev/?p=165 where:
	 * - Only one Resistance attribute is used for ALL Armor / resistance.
	 *
	 * Note: this Exec cannot handle more than one damage type at the same type, as we are relying on the Asset tags to calculate the correct resistance.
	 */

	float CurrentResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics->ResistanceDef, EvaluationParameters, CurrentResistance);
	CurrentResistance = FMath::Clamp(CurrentResistance, UResistanceAttributeSet::RESISTANCE_MIN, UResistanceAttributeSet::RESISTANCE_MAX);

	const float Resistance_DamageMultiplier = UCustomAbilitySystemBlueprintLibrary::CalculateEffectiveResistance(CurrentResistance);

	// Calculate the final damage
	const float FinalDamage = DamageRaw * Resistance_DamageMultiplier;

	// It's too close to 0 to consider anything, return early.
	if (FinalDamage <= 0.f || FMath::IsNearlyZero(DamageRaw, 0.01f))
	{
		return;
	}

	// Output the final damage to our player.
	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			DamageStatics->CurrentHealthProperty,
			EGameplayModOp::Additive,
			-FinalDamage
			)
	);
}