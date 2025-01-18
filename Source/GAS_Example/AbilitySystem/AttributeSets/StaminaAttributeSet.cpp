// Copyright 2021 Joseph "Narxim" Thigpen.


#include "StaminaAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GAS_Example/AbilitySystem/Data/NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

UStaminaAttributeSet::UStaminaAttributeSet()
{
	MaximumStamina = 1.0f;
	CurrentStamina = 1.0f;
	StaminaRegeneration = 0.0f;
}

void UStaminaAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue,	float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (Attribute == GetCurrentStaminaAttribute())
	{
		CheckMaxReachedForAttribute(MaximumStamina, NativeGameplayTags::State::TAG_State_Max_Stamina.GetTag(), NewValue);
		return;
	}

	if (Attribute == GetMaximumStaminaAttribute())
	{
		AdjustAttributeForMaxChange(GetCurrentStaminaAttribute(), OldValue, NewValue);
		return;
	}
}

void UStaminaAttributeSet::ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::ClampAttributes(Attribute, NewValue);
	
	if (Attribute == GetCurrentStaminaAttribute())
	{
		// This should be removed in favor of another method, as we're modifying 2 times the current stamina. (One right before post, one right after (here).
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaximumStamina());
		return;
	}

	if (Attribute == GetStaminaRegenerationAttribute())
	{
		NewValue = FMath::Max(0.f, NewValue);
		return;
	}
}

void UStaminaAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params{};
	Params.bIsPushBased = true;
	Params.Condition = COND_None;

	// Replicated to all
	DOREPLIFETIME_WITH_PARAMS_FAST(UStaminaAttributeSet, CurrentStamina, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(UStaminaAttributeSet, MaximumStamina, Params);

	// Owner Only
	Params.Condition = COND_OwnerOnly;
	DOREPLIFETIME_WITH_PARAMS_FAST(UStaminaAttributeSet, StaminaRegeneration, Params);
}

void UStaminaAttributeSet::OnRep_CurrentStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStaminaAttributeSet, CurrentStamina, OldValue);
}

void UStaminaAttributeSet::OnRep_MaximumStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStaminaAttributeSet, MaximumStamina, OldValue);
}

void UStaminaAttributeSet::OnRep_StaminaRegeneration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStaminaAttributeSet, StaminaRegeneration, OldValue);
}