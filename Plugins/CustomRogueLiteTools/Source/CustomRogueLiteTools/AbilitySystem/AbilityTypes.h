// Copyright 2021 Joseph "Narxim" Thigpen.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "AbilityTypes.generated.h"

class UCRLAbility;

USTRUCT(BlueprintType)
struct FCRLChangedValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttribute ChangedAttribute;

	// Can be positive or negative.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value = 0.f;

	/*
	 * Default = 1. Will not be clamped. It is the responsibility of the GA / Dev to make this value OK.
	 * Will be applied at the end.
	 */ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Multiplier = 1.f;

	bool IsValid(const float Tolerance = 0.1f) const
	{
		return FMath::IsNearlyZero(Value, Tolerance) || FMath::IsNearlyZero(Multiplier, Tolerance);
	}

	float GetMagnitude() const
	{
		return FMath::Sign(Value) * FMath::Max(Value, 0.f) * FMath::Max(Multiplier, 0.f);
	}
};

USTRUCT(BlueprintType)
struct FCRLChangedAttributeCollection
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer SourceTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer TargetTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, FCRLChangedValue> ChangedAttributeCollection{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<const UAbilitySystemComponent> TargetASC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<const AActor> TargetActor;
};

UENUM(Blueprintable)
enum class ECRLModifierEvent : uint8
{
	PreDamageCalculation,
	PostDamageCalculation,
	PreMitigation,
	PostMitigation
};