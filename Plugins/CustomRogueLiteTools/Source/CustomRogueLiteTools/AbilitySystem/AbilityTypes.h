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
	float ValueChange = 0.f;

	/*
	 * Default = 1. Will not be clamped. It is the responsibility of the GA / Dev to make this value OK.
	 * Will be applied at the end.
	 */ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PercentageChange = 1.f;
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
};

UENUM(Blueprintable)
enum class ECRLModifierEvent : uint8
{
	PreDamageCalculation,
	PostDamageCalculation,
	PreMitigation,
	PostMitigation
};