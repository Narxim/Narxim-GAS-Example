// Copyright 2021 Joseph "Narxim" Thigpen.


#include "AbilitySystemFunctionLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GAS_Example/AbilitySystem/Data/AbilitySystemData.h"


float UAbilitySystemFunctionLibrary::GetAttributeValueFromActor(const AActor* Actor, const FGameplayAttribute Attribute, const EAttributeSearchType SearchType)
{
	float ReturnValue = -1.0f;
	
	if (const UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		GetAttributeValue(AbilitySystemComponent, Attribute, SearchType, ReturnValue);
	}

	return ReturnValue;
}

float UAbilitySystemFunctionLibrary::GetAttributeValueFromAbilitySystem(const UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAttribute Attribute, const EAttributeSearchType SearchType)
{
	float ReturnValue = -1.0f;

	GetAttributeValue(AbilitySystemComponent, Attribute, SearchType, ReturnValue);

	return ReturnValue;
}

void UAbilitySystemFunctionLibrary::GetAttributeValue(const UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAttribute& Attribute, const EAttributeSearchType SearchType, float& ReturnValue)
{
	ReturnValue = -1.0f;
	
	if (!AbilitySystemComponent || !AbilitySystemComponent->HasAttributeSetForAttribute(Attribute))
	{
		return;
	}

	switch (SearchType)
	{
		
	case EAttributeSearchType::FinalValue :
		{
			ReturnValue = AbilitySystemComponent->GetNumericAttribute(Attribute);

			return;
		}

	case EAttributeSearchType::BaseValue :
		{
			ReturnValue = AbilitySystemComponent->GetNumericAttributeBase(Attribute);

			return;
		}

	case EAttributeSearchType::BonusValue :
		{
			ReturnValue = AbilitySystemComponent->GetNumericAttribute(Attribute) - AbilitySystemComponent->GetNumericAttributeBase(Attribute);
		}
	}
}
