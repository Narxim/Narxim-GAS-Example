// Copyright 2021 Joseph "Narxim" Thigpen.


#include "TurnBasedAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UTurnBasedAttributeSet::UTurnBasedAttributeSet()
{
	TurnBasedModifier = 1.0f;
}

void UTurnBasedAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	NewValue = 1.f;
}

void UTurnBasedAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params{};
	Params.bIsPushBased = true;
	Params.Condition = COND_OwnerOnly;

	// Replicated to all
	DOREPLIFETIME_WITH_PARAMS_FAST(UTurnBasedAttributeSet, TurnBasedModifier, Params);
}

void UTurnBasedAttributeSet::OnRep_TurnBasedModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTurnBasedAttributeSet, TurnBasedModifier, OldValue);
}