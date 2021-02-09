// Copyright 2021 Joseph "Narxim" Thigpen.


#include "AttributeSets/StaminaAttributeSet.h"
#include "GameplayEffectExtension.h"
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

	if (Attribute == GetMaximumStaminaAttribute())
	{
		AdjustAttributeForMaxChange(CurrentStamina, MaximumStamina, NewValue, GetCurrentStaminaAttribute());
	}
}

void UStaminaAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetCurrentStaminaAttribute())
	{
		SetCurrentStamina(FMath::Clamp(GetCurrentStamina(), 0.0f, GetMaximumStamina()));
	}

	else if (Data.EvaluatedData.Attribute == GetStaminaRegenerationAttribute())
	{
		SetStaminaRegeneration(FMath::Clamp(GetStaminaRegeneration(), 0.0f, GetMaximumStamina()));
	}
}

void UStaminaAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UStaminaAttributeSet, CurrentStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UStaminaAttributeSet, MaximumStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UStaminaAttributeSet, StaminaRegeneration, COND_None, REPNOTIFY_Always);
}

void UStaminaAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const
{
	UAbilitySystemComponent* AbilitySystemComponent = GetOwningAbilitySystemComponent();
	
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilitySystemComponent)
	{
		// Change current value to maintain the Current Value / Maximum Value percentage.
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		const float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilitySystemComponent->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
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