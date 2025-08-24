// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AttributeSet.h"
#include "CustomAbilitySystemBlueprintLibrary.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
class UAttributeSet;

enum class EAttributeSearchType : uint8;

// Contains helper functions that make interacting with the Gameplay Abilities System easier.
UCLASS()
class GAS_EXAMPLE_API UCustomAbilitySystemBlueprintLibrary : public UAbilitySystemBlueprintLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Ability System")
	static AActor* GetInstigatorFromGameplayEffectSpec(const FGameplayEffectSpec& Spec);
	
	/*
	* Tries to find the Actor's Ability System Component using the IAbilitySystemInterface.
	* If the Ability System Component is found; attempts to find the value of the Attribute supplied.
	*/
	UFUNCTION(BlueprintCallable, Category = "Ability System")
	static float GetAttributeValueFromActor(const AActor* const Actor, const FGameplayAttribute Attribute, const EAttributeSearchType SearchType);
	
	// Attempts to find the value of the Attribute supplied.
	UFUNCTION(BlueprintCallable, Category = "Ability System")
	static float GetAttributeValueFromAbilitySystem(const UAbilitySystemComponent* const AbilitySystemComponent, const FGameplayAttribute Attribute, const EAttributeSearchType SearchType);

	// Return damage multiplier
	UFUNCTION(BlueprintCallable, Category = "Ability System")
	static float CalculateEffectiveResistance(const float CurrentArmor);

	UFUNCTION(BlueprintPure, Category = "Ability System", meta=(AutoCreateRefTerm="ContextString"))
	static float GetValueAtLevel(const FScalableFloat& ScalableFloat, const float Level = 0.f, const FString& ContextString = "");

	UFUNCTION(BlueprintCallable, Category = "Ability System", meta=(AutoCreateRefTerm="ContextString"))
	static void SetTargetOnGameplayEffectContext(FGameplayEffectContextHandle& ContextHandle, const AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Ability System", meta=(AutoCreateRefTerm="ContextString"))
	static void SetTargetOnGameplayEffectContextFromSpec(FGameplayEffectSpec& EffectSpec, const AActor* TargetActor);

	UFUNCTION(BlueprintPure, Category = "Ability System", meta=(AutoCreateRefTerm="ContextString"))
	static const AActor* GetTargetActorFromGameplayEffectContext(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintPure, Category = "Ability System", meta=(AutoCreateRefTerm="ContextString"))
	static const AActor* GetTargetActorFromGameplayEffectSpec(const FGameplayEffectSpec& EffectSpec);
private:

	static void GetAttributeValue(const UAbilitySystemComponent* const AbilitySystemComponent, const FGameplayAttribute& Attribute, const EAttributeSearchType SearchType, OUT float& ReturnValue);
};