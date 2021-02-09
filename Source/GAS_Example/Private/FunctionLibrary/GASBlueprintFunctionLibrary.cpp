// Copyright 2021 Joseph "Narxim" Thigpen.


#include "FunctionLibrary/GASBlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

UAbilitySystemComponent* UGASBlueprintFunctionLibrary::TryGetAbilitySystemComponentFromActor(AActor* Actor, bool& WasFound)
{
	const bool ImplementsInterface = UKismetSystemLibrary::DoesImplementInterface(Actor, UAbilitySystemInterface::StaticClass());

	if (ImplementsInterface)
	{
		IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(Actor);
		
		WasFound = true;
		
		return Interface->GetAbilitySystemComponent();
	}

	WasFound = false;
	
	return nullptr;
}

float UGASBlueprintFunctionLibrary::TryGetAttributeValueFromActor(AActor* Actor, FGameplayAttribute Attribute, EAttributeSearchType SearchType, bool& ValueFound)
{
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;
	
	const bool ImplementsInterface = UKismetSystemLibrary::DoesImplementInterface(Actor, UAbilitySystemInterface::StaticClass());

	if (ImplementsInterface)
	{
		AbilitySystemComponent = Cast<IAbilitySystemInterface>(Actor)->GetAbilitySystemComponent();
	}

	if (AbilitySystemComponent && AbilitySystemComponent->HasAttributeSetForAttribute(Attribute))
	{
		switch (SearchType)
		{
			case FinalValue :
				{
					ValueFound = true;
					
					return AbilitySystemComponent->GetNumericAttribute(Attribute);
				}

			case BaseValue :
				{
					ValueFound = true;
					
					return AbilitySystemComponent->GetNumericAttributeBase(Attribute);
				}

			case BonusValue :
				{
					ValueFound = true;
					
					return AbilitySystemComponent->GetNumericAttribute(Attribute) - AbilitySystemComponent->GetNumericAttributeBase(Attribute);
				}
		}
	}

	ValueFound = false;

	return -1.0f;
}