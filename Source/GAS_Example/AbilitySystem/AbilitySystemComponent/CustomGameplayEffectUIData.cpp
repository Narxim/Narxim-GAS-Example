// Copyright 2021 Joseph "Narxim" Thigpen.


#include "CustomGameplayEffectUIData.h"

#include "AbilitySystemComponent.h"
#include "ActiveGameplayEffectHandle.h"

const UCustomGameplayEffectUIData* UCustomGameplayEffectUIData::GetCustomGameplayEffectUIData(FActiveGameplayEffectHandle ActiveEffectHandle)
{
	const UAbilitySystemComponent* const ASC = ActiveEffectHandle.GetOwningAbilitySystemComponent();

	if (!ASC)
	{
		return nullptr;
	}

	const FActiveGameplayEffect* const Effect = ASC->GetActiveGameplayEffect(ActiveEffectHandle);

	if (!Effect)
	{
		return nullptr;
	}

	return GetCustomGameplayEffectUIDataFromActiveEffect(*Effect);
}

const UCustomGameplayEffectUIData* UCustomGameplayEffectUIData::GetCustomGameplayEffectUIDataFromActiveEffect(const FActiveGameplayEffect& ActiveEffect)
{
	const UCustomGameplayEffectUIData* const UIData = ActiveEffect.Spec.Def->FindComponent<UCustomGameplayEffectUIData>();
	
	return UIData;
}
