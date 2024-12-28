// Copyright 2021 Joseph "Narxim" Thigpen.


#include "ResistanceAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

float UResistanceAttributeSet::RESISTANCE_BASE = 100.f; 
float UResistanceAttributeSet::RESISTANCE_MAX = 200.f; // 3 times the base armor.
float UResistanceAttributeSet::RESISTANCE_MIN = -75.f; // Up to 1/4 of the base armor.

UResistanceAttributeSet::UResistanceAttributeSet()
{
	Resistance = 0.0f;
}

void UResistanceAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetResistanceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, RESISTANCE_MIN, RESISTANCE_MAX);
		return;
	}
}

void UResistanceAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	if (Attribute == GetResistanceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, RESISTANCE_MIN, RESISTANCE_MAX);
		return;
	}
}

void UResistanceAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UResistanceAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UResistanceAttributeSet::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);
}

void UResistanceAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params{};
	Params.bIsPushBased = true;
	Params.Condition = COND_None;

	// Replicated to all
	DOREPLIFETIME_WITH_PARAMS_FAST(UResistanceAttributeSet, Resistance, Params);
}

void UResistanceAttributeSet::OnRep_Resistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResistanceAttributeSet, Resistance, OldValue);
}