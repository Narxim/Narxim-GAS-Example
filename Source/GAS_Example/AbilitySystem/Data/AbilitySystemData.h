// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemData.generated.h"

class UAttributeSet;
class UGameplayAbility;
class UGameplayEffect;

// Contains data used to initialize an Ability System Component.
USTRUCT(BlueprintType)
struct FAbilitySystemInitializationData
{
	GENERATED_BODY()

	// An array of Attribute Sets to create.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<TSubclassOf<UAttributeSet>> AttributeSets;

	// A map of Attributes / float used to set base values.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<FGameplayAttribute, float> AttributeBaseValues;

	// An Array of Gameplay Abilities to give.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<TSubclassOf<UGameplayAbility>> GameplayAbilities;

	// An array of Gameplay Effects to apply.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffects;

	// A container of GameplayTags to apply.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTagContainer GameplayTags;
};

// Returns the value of an Attribute based on the search type.
UENUM(BlueprintType)
enum class EAttributeSearchType : uint8
{
	// Returns the final value of the Attribute including all stateful Gameplay Effect modifiers.
	FinalValue,

	// Returns the base value of the Attribute. (Excludes duration based Gameplay Effect modifiers)
	BaseValue,

	// Returns the Final Value minus the Base Value.
	BonusValue
};