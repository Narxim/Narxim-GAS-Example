// Copyright 2021 Joseph "Narxim" Thigpen.


#include "PlayerControllerBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "GAS_Example/AbilitySystem/AbilitySystemComponent/CustomAbilitySystemComponent.h"
#include "GAS_Example/Characters/CharacterBase.h"

UAbilitySystemComponent* APlayerControllerBase::GetAbilitySystemComponent() const
{
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner.Get());
}