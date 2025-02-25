// Copyright 2021 Joseph "Narxim" Thigpen.


#include "CustomAbilitySystemGlobals.h"

#include "Data/AbilitySystemData.h"

FGameplayEffectContext* UCustomAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FCustomGameplayEffectContext();
}

FGameplayAbilityActorInfo* UCustomAbilitySystemGlobals::AllocAbilityActorInfo() const
{
	return Super::AllocAbilityActorInfo();
}
