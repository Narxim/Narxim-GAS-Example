// Copyright 2021 Joseph "Narxim" Thigpen.


#include "StaminaAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GAS_Example/AbilitySystem/Data/NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

UStaminaAttributeSet::UStaminaAttributeSet()
{
	MaximumStamina = 0.0f;
	CurrentStamina = 0.0f;
	StaminaRegeneration = 0.0f;
}

void UStaminaAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UStaminaAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetCurrentStaminaAttribute())
	{
		// This should be removed in favor of another method, as we're modifying 2 times the current stamina. (One right before post, one right after (here).
		SetCurrentStamina(FMath::Clamp(GetCurrentStamina(), 0.0f, GetMaximumStamina()));
		return;
	}

	if (Data.EvaluatedData.Attribute == GetStaminaRegenerationAttribute())
	{
		SetStaminaRegeneration(FMath::Clamp(GetStaminaRegeneration(), 0.0f, GetMaximumStamina()));
		return;
	}
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