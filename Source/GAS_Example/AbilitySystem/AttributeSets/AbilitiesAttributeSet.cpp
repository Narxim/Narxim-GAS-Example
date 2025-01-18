// Copyright 2021 Joseph "Narxim" Thigpen.


#include "AbilitiesAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

float UAbilitiesAttributeSet::COOLDOWN_MULTIPLIER_MIN = 0.1f;
float UAbilitiesAttributeSet::COOLDOWN_MULTIPLIER_MAX = 2.f;

float UAbilitiesAttributeSet::COST_MULTIPLIER_MIN = 0.1f;
float UAbilitiesAttributeSet::COST_MULTIPLIER_MAX = 2.f;

UAbilitiesAttributeSet::UAbilitiesAttributeSet()
{
	InitCooldownMultiplier(1.f);
	InitCostMultiplier(1.f);
}

void UAbilitiesAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UAbilitiesAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UAbilitiesAttributeSet::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);
}

void UAbilitiesAttributeSet::ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetCostMultiplierAttribute())
	{
		NewValue = FMath::Clamp(NewValue, COST_MULTIPLIER_MIN, COST_MULTIPLIER_MAX);
		return;
	}

	if (Attribute == GetCooldownMultiplierAttribute())
	{
		NewValue = FMath::Clamp(NewValue, COOLDOWN_MULTIPLIER_MIN, COOLDOWN_MULTIPLIER_MAX);
		return;
	}
}

void UAbilitiesAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params{};
	Params.bIsPushBased = true;
	Params.Condition = COND_OwnerOnly;

	// Replicated to all
	DOREPLIFETIME_WITH_PARAMS_FAST(UAbilitiesAttributeSet, CooldownMultiplier, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(UAbilitiesAttributeSet, CostMultiplier, Params);
}

void UAbilitiesAttributeSet::OnRep_CooldownMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAbilitiesAttributeSet, CooldownMultiplier, OldValue);
}

void UAbilitiesAttributeSet::OnRep_CostMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAbilitiesAttributeSet, CostMultiplier, OldValue);
}