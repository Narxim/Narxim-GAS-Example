// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GAS_Example/AbilitySystem/AttributeSets/AttributeSetBase.h"
#include "CRLAttributeSet.generated.h"

UCLASS()
class CUSTOMROGUELITETOOLS_API UCRLAttributeSet : public UAttributeSetBase
{
	GENERATED_BODY()

public:
	
	UCRLAttributeSet();
	
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	// Attribute Set Overrides.
	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const override;

	virtual void ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const override;

	// Set Attributes to replicate.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Multiplier;
	ATTRIBUTE_ACCESSORS(UCRLAttributeSet, Multiplier)

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData DamageMultiplier;
	ATTRIBUTE_ACCESSORS(UCRLAttributeSet, DamageMultiplier)

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData CriticalChance;
	ATTRIBUTE_ACCESSORS(UCRLAttributeSet, CriticalChance)

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData CriticalDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UCRLAttributeSet, CriticalDamageMultiplier)

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData ResistanceMultiplier;
	ATTRIBUTE_ACCESSORS(UCRLAttributeSet, ResistanceMultiplier)

protected:
};