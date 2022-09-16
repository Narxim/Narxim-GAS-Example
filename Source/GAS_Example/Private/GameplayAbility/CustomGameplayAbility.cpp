// Copyright 2021 Joseph "Narxim" Thigpen.


#include "GameplayAbility/CustomGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "FunctionLibrary/AbilitySystemFunctionLibrary.h"

UCustomGameplayAbility::UCustomGameplayAbility()
{
	// Sets the ability to default to Instanced Per Actor.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;	
}

void UCustomGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (ActivateAbilityOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}

float UCustomGameplayAbility::GetModifiedAbilityStrength()
{
	float BaseStrength = BaseAbilityStrength.GetValueAtLevel(GetAbilityLevel());

	if (const UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
	{
		for (const FAbilityModifierStruct Modifier : AbilityModifiers)
		{
			if (const float AttributeValue = UAbilitySystemFunctionLibrary::GetAttributeValueFromAbilitySystem(AbilitySystemComponent, Modifier.BackingAttribute, Modifier.AttributeSearchType) != -1.0f)
			{
				BaseStrength += AttributeValue * Modifier.AttributeModifier;
			}
		}
	}
	
	return BaseStrength;
}