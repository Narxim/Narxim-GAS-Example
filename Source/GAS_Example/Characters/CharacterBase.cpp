// Copyright 2021 Joseph "Narxim" Thigpen.


#include "CharacterBase.h"
#include "GAS_Example/AbilitySystem/AbilitySystemComponent/CustomAbilitySystemComponent.h"


UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UCustomAbilitySystemComponent* ACharacterBase::GetCustomAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}
