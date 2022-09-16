// Copyright 2021 Joseph "Narxim" Thigpen.


#include "FunctionLibrary/AbilitySystemFunctionLibrary.h"
#include "Data/AbilitySystemData.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayAbility/CustomGameplayAbility.h"

bool UAbilitySystemFunctionLibrary::GiveGameplayAbilityToActor(const AActor* Actor, const TSubclassOf<UGameplayAbility> AbilityToGive, const int32 Level)
{
	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	
	return GiveGameplayAbility(AbilitySystemComponent, AbilityToGive, Level);
}

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

bool UAbilitySystemFunctionLibrary::GiveGameplayAbility(UAbilitySystemComponent* AbilitySystemComponent, const TSubclassOf<UGameplayAbility> AbilityToGive, const int32 Level)
{
	if (!AbilitySystemComponent || !AbilityToGive)
	{
		return false;
	}

	// Checking here to see if the Gameplay Ability uses the Custom Gameplay Ability class so that it can be bound to its input binding. 
	if (UCustomGameplayAbility* CustomGameplayAbility = Cast<UCustomGameplayAbility>(AbilityToGive))
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(CustomGameplayAbility, Level, static_cast<uint32>(CustomGameplayAbility->InputBinding), nullptr));

		return true;
	}
	
	AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityToGive, Level));

	return true;
}

void UAbilitySystemFunctionLibrary::GetAttributeValue(const UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAttribute Attribute, const EAttributeSearchType SearchType, float& ReturnValue)
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
