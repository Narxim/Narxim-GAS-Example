// Copyright 2021 Joseph "Narxim" Thigpen.


#include "CRLAttributeSet.h"
#include "GameplayEffectExtension.h"

UCRLAttributeSet::UCRLAttributeSet()
{
	Multiplier = 1.f;
	DamageMultiplier = 1.f;
	ResistanceMultiplier = 0.f;
}

void UCRLAttributeSet::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);
}

void UCRLAttributeSet::ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::ClampAttributes(Attribute, NewValue);

	if (Attribute == GetMultiplierAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
		return;
	}
}

void UCRLAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// FDoRepLifetimeParams Params{};
	// Params.bIsPushBased = true;
	// Params.Condition = COND_None;
	//
	// // Replicated to all
	// DOREPLIFETIME_WITH_PARAMS_FAST(UCustomRogueLiteAttributeSet, Resistance, Params);
}