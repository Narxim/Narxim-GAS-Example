// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSetBase.h"
#include "TurnBasedAttributeSet.generated.h"


/* Contains Attributes related to TurnBase.
 * Contents:
 * TurnBaseMultipliers: Base Multipliers for any turnbase buff to:
 * - Inhibition Turns
 * - Duration Turns
 * - Periodic Applications ...
 */

UCLASS()
class GAS_EXAMPLE_API UTurnBasedAttributeSet : public UAttributeSetBase
{
	GENERATED_BODY()

public:
	UTurnBasedAttributeSet();
	
	// Attribute Set Overrides.
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	// Set Attributes to replicate.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Turned Based Attribute Set", ReplicatedUsing = OnRep_TurnBasedModifier)
	FGameplayAttributeData TurnBasedModifier;
	ATTRIBUTE_ACCESSORS(UTurnBasedAttributeSet, TurnBasedModifier)

protected:
	
	UFUNCTION()
	virtual void OnRep_TurnBasedModifier(const FGameplayAttributeData& OldValue);
};