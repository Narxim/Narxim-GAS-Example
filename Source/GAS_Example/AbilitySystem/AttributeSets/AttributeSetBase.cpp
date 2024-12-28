// Copyright 2021 Joseph "Narxim" Thigpen.


#include "AttributeSetBase.h"
#include "AbilitySystemComponent.h"


void UAttributeSetBase::AdjustAttributeForMaxChange(const FGameplayAttribute& AffectedAttribute, const float OldMaxValue, const float NewMaxValue) const
{
	UAbilitySystemComponent* const ASC = GetOwningAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}
	
	if (OldMaxValue <= 0.f || FMath::IsNearlyEqual(OldMaxValue, NewMaxValue, 0.f))
	{
		return;
	}
	
	// Change current value to maintain the Current Value / Maximum Value percentage.
	ASC->SetNumericAttributeBase(AffectedAttribute, ASC->GetNumericAttributeBase(AffectedAttribute) * NewMaxValue / OldMaxValue);
}

void UAttributeSetBase::CheckMaxReachedForAttribute(const FGameplayAttributeData& MaxAttribute, const FGameplayTag& MaxTag, const float& NewValue) const
{
	UAbilitySystemComponent* const ASC = GetOwningAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	const float MaxHealth = MaxAttribute.GetCurrentValue();
	const bool bHasTag = GetOwningAbilitySystemComponent()->HasMatchingGameplayTag(MaxTag);

	int32 Count = -1;
	if (NewValue < MaxHealth && bHasTag)
	{
		Count = 0;
	} else if (NewValue >= MaxHealth && !bHasTag)
	{
		Count = 1;
	}

	if (Count >= 0)
	{
		ASC->SetLooseGameplayTagCount(MaxTag, Count);
		ASC->SetReplicatedLooseGameplayTagCount(MaxTag, Count);
	}
}

void UAttributeSetBase::CheckStatusTagForAttribute(const FGameplayTag& StatusTag, const float& NewValue, const float& OldValue) const
{
	UAbilitySystemComponent* const ASC = GetOwningAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}
	
	int32 Count = -1;
		
	if (NewValue <= 0.f && OldValue > 0.f)
	{
		Count = 0;
	}
	else if (NewValue > 0.f && OldValue <= 0.f)
	{
		Count = 1;
	}

	if (Count >= 0)
	{
		ASC->SetLooseGameplayTagCount(StatusTag, Count);
		ASC->SetReplicatedLooseGameplayTagCount(StatusTag, Count);
	}
}
