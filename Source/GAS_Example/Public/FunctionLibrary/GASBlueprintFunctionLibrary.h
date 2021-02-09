// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GASBlueprintFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum EAttributeSearchType
{
	FinalValue,
	BaseValue,
	BonusValue
};

// Contains helper functions that make interacting with the Gameplay Abilities System easier.

UCLASS()
class GAS_EXAMPLE_API UGASBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// Uses the IAbilitySystemInterface to find the Ability System Component associated with the supplied Actor.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability System")
	static class UAbilitySystemComponent* TryGetAbilitySystemComponentFromActor(AActor* Actor, bool& WasFound); 

	// Tries to find the Actor's Ability System Component using the IAbilitySystemInterface.
	// If the Ability System Component is found, attempts to find the value of the Attribute supplied.
	//
	// -Search Type-
	// 
	// Final Value: Returns the final value of the Attribute including all stateful Gameplay Effect modifiers.
	// Base Value: Returns the base value of the Attribute. (Excludes duration based Gameplay Effect modifiers)
	// Bonus Value: Returns the Final Value minus the Base Value.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability System")
	static float TryGetAttributeValueFromActor(AActor* Actor, FGameplayAttribute Attribute, EAttributeSearchType SearchType, bool& ValueFound);
};