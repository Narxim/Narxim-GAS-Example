// Copyright 2021 Joseph "Narxim" Thigpen.


#include "CustomGameplayEffectApplicationRequirement.h"
#include "GameplayEffect.h"

UCustomGameplayEffectApplicationRequirement::UCustomGameplayEffectApplicationRequirement()
{
#if WITH_EDITORONLY_DATA
	EditorFriendlyName = TEXT("Custom Requirement (Change the name please)");
#endif // WITH_EDITORONLY_DATA
}

bool UCustomGameplayEffectApplicationRequirement::CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec) const
{
	return K0_CanApplyGameplayEffect(GESpec.Def, GESpec, ActiveGEContainer.Owner);
}

#if WITH_EDITOR
EDataValidationResult UCustomGameplayEffectApplicationRequirement::IsDataValid(
	class FDataValidationContext& Context) const
{
	const UGameplayEffect* const Owner = GetOwner();
	if (!Owner)
	{
		return EDataValidationResult::NotValidated;
	}
	
	return Super::IsDataValid(Context);
}
#endif

bool UCustomGameplayEffectApplicationRequirement::K0_CanApplyGameplayEffect_Implementation(
	const UGameplayEffect* const GameplayEffect,
	const FGameplayEffectSpec& Spec,
	const UAbilitySystemComponent* const ASC) const
{
	return true;
}
