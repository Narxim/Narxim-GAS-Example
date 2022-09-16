// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AttributeSet.h"
#include "AbilitySystemFunctionLibrary.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
class UAttributeSet;

enum class EAttributeSearchType : uint8;

// Contains helper functions that make interacting with the Gameplay Abilities System easier.

UCLASS()
class GAS_EXAMPLE_API UAbilitySystemFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// Tries to find the Actor's Ability System Component using the IAbilitySystemInterface.
	// If the Ability System Component is found; attempts to give the specified Gameplay Ability to the Actor.
	//
	// Note: Should only be called by the server or local host! (Blueprint Authority Only)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ability System")
	static bool GiveGameplayAbilityToActor(const AActor* Actor, const TSubclassOf<UGameplayAbility> AbilityToGive, const int32 Level = 1);
	
	// Tries to find the Actor's Ability System Component using the IAbilitySystemInterface.
	// If the Ability System Component is found; attempts to find the value of the Attribute supplied.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability System")
	static float GetAttributeValueFromActor(const AActor* Actor, const FGameplayAttribute Attribute, const EAttributeSearchType SearchType);
	
	// Attempts to find the value of the Attribute supplied.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability System")
	static float GetAttributeValueFromAbilitySystem(const UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAttribute Attribute, const EAttributeSearchType SearchType);

private:

	static bool GiveGameplayAbility(UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> AbilityToGive, const int32 Level);
	
	static void GetAttributeValue(const UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAttribute Attribute, const EAttributeSearchType SearchType, float& ReturnValue);
};