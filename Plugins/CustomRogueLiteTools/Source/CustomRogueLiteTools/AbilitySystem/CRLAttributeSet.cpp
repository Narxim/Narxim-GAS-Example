// Copyright 2021 Joseph "Narxim" Thigpen.


#include "CRLAttributeSet.h"
#include "GameplayEffectExtension.h"

UCRLAttributeSet::UCRLAttributeSet()
{
	Multiplier = 1.f;
	DamageMultiplier = 1.f;
	ResistanceMultiplier = 0.f;
	CriticalChance = 0.f;
	CriticalDamageMultiplier = 0.f;
}

void UCRLAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// BaseValues can never be changed (because it would mean applying something for ALL attacks.)
	
	if (Attribute == GetMultiplierAttribute())
	{
		NewValue = 1.f;
	}
	else if (Attribute == GetResistanceMultiplierAttribute())
	{
		NewValue = 1.f;
	}
	else if (Attribute == GetCriticalChanceAttribute())
	{
		NewValue = 0.f;
	}
	else if (Attribute == GetCriticalDamageMultiplierAttribute())
	{
		NewValue = 0.f;
	}
	
	Super::PreAttributeBaseChange(Attribute, NewValue);
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

	if (Attribute == GetResistanceMultiplierAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
		return;
	}

	if (Attribute == GetCriticalChanceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 1.f);
		return;
	}

	if (Attribute == GetCriticalDamageMultiplierAttribute())
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