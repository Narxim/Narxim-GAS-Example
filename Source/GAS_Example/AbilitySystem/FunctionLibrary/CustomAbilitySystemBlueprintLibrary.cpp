// Copyright 2021 Joseph "Narxim" Thigpen.


#include "CustomAbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GAS_Example/AbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "GAS_Example/AbilitySystem/AttributeSets/ResistanceAttributeSet.h"
#include "GAS_Example/AbilitySystem/Data/AbilitySystemData.h"


float UCustomAbilitySystemBlueprintLibrary::GetAttributeValueFromActor(const AActor* const Actor, const FGameplayAttribute Attribute, const EAttributeSearchType SearchType)
{
	const UAbilitySystemComponent* const AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	
	if (!AbilitySystemComponent)
	{
		return -1.f;
	}

	float ReturnValue = -1.0f;
	GetAttributeValue(AbilitySystemComponent, Attribute, SearchType, ReturnValue);
	
	return ReturnValue;
}

float UCustomAbilitySystemBlueprintLibrary::GetAttributeValueFromAbilitySystem(const UAbilitySystemComponent* const AbilitySystemComponent, const FGameplayAttribute Attribute, const EAttributeSearchType SearchType)
{
	float ReturnValue = -1.0f;

	GetAttributeValue(AbilitySystemComponent, Attribute, SearchType, ReturnValue);

	return ReturnValue;
}

float UCustomAbilitySystemBlueprintLibrary::CalculateEffectiveResistance(const float CurrentArmor)
{
	const float CurrentArmorTmp = FMath::Clamp(CurrentArmor, UResistanceAttributeSet::RESISTANCE_MIN, UResistanceAttributeSet::RESISTANCE_MAX);
	
	const float ArmorDenominator = UResistanceAttributeSet::RESISTANCE_BASE + CurrentArmorTmp;
	return UResistanceAttributeSet::RESISTANCE_BASE > 0.f && ArmorDenominator > 0.f ? UResistanceAttributeSet::RESISTANCE_BASE / ArmorDenominator : 1.f;
}

float UCustomAbilitySystemBlueprintLibrary::GetValueAtLevel(const FScalableFloat& ScalableFloat, const float Level /* = 0.f */,	const FString& ContextString /* = "" */)
{
	return ScalableFloat.GetValueAtLevel(Level, &ContextString);
}

void UCustomAbilitySystemBlueprintLibrary::GetAttributeValue(const UAbilitySystemComponent* const AbilitySystemComponent, const FGameplayAttribute& Attribute, const EAttributeSearchType SearchType, OUT float& ReturnValue)
{
	ReturnValue = -1.0f;
	
	if (!AbilitySystemComponent || !AbilitySystemComponent->HasAttributeSetForAttribute(Attribute))
	{
		return;
	}

	switch (SearchType)
	{
	case EAttributeSearchType::FinalValue:
		{
			ReturnValue = AbilitySystemComponent->GetNumericAttribute(Attribute);

			return;
		}

	case EAttributeSearchType::BaseValue:
		{
			ReturnValue = AbilitySystemComponent->GetNumericAttributeBase(Attribute);

			return;
		}

	case EAttributeSearchType::BonusValue:
		{
			ReturnValue = AbilitySystemComponent->GetNumericAttribute(Attribute) - AbilitySystemComponent->
				GetNumericAttributeBase(Attribute);
		}
	}
}
