// Copyright 2021 Joseph "Narxim" Thigpen.


#include "PlayerControllerBase.h"
#include "AbilitySystemGlobals.h"

UAbilitySystemComponent* APlayerControllerBase::GetAbilitySystemComponent() const
{
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn());
}
