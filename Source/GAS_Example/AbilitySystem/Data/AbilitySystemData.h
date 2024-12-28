// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "AbilitySystemData.generated.h"


struct FCustomAttributeInitializer;
class UAttributeSet;
class UGameplayAbility;
class UGameplayEffect;

USTRUCT(Blueprintable)
struct FCustomAttributeMaxValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FScalableFloat MaxFloat{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttribute MaxAttribute{};
};

USTRUCT(Blueprintable)
struct FCustomAttributeInitializer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FScalableFloat Value;
};

// Contains data used to initialize an Ability System Component.
USTRUCT(BlueprintType)
struct FAbilitySystemInitializationData
{
	GENERATED_BODY()

	// An array of Attribute Sets to create.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSet<TSubclassOf<UAttributeSet>> AttributeSets;

	// A map of Attributes / float used to set base values.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<FGameplayAttribute, FCustomAttributeInitializer> AttributeBaseValues;

	// An Array of Gameplay Abilities to give.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSet<TSubclassOf<UGameplayAbility>> GameplayAbilities;

	// An array of Gameplay Effects to apply.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSet<TSubclassOf<UGameplayEffect>> GameplayEffects;

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