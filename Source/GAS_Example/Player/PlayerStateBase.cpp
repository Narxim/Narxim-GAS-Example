// Copyright 2021 Joseph "Narxim" Thigpen.


#include "PlayerStateBase.h"
#include "AbilitySystemBlueprintLibrary.h"


APlayerStateBase::APlayerStateBase()
{
}

UAbilitySystemComponent* APlayerStateBase::GetAbilitySystemComponent() const
{
	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
}