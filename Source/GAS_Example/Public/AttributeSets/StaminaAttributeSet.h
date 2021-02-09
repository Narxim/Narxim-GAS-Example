// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "StaminaAttributeSet.generated.h"

// Use macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// Contains Attributes related to Stamina. Any Character that uses Stamina will need this Attribute Set.
//
//	Contents:
//	
//	CurrentStamina - Primarily used as a cost resource for Abilities.
//	MaximumStamina - The maximum amount of Stamina the Character can have.
//	StaminaRegeneration - How much Stamina is gained per period of Regeneration.

UCLASS()
class GAS_EXAMPLE_API UStaminaAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	
	UStaminaAttributeSet();

	// Attribute Set Overrides.
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// Set Attributes to replicate.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Holds the current value for Stamina.
	UPROPERTY(BlueprintReadOnly, Category = "Stamina Attribute Set", ReplicatedUsing = OnRep_CurrentStamina)
	FGameplayAttributeData CurrentStamina;
	ATTRIBUTE_ACCESSORS(UStaminaAttributeSet, CurrentStamina)

	// Holds the value for Maximum Stamina.
	UPROPERTY(BlueprintReadOnly, Category = "Stamina Attribute Set", ReplicatedUsing = OnRep_MaximumStamina)
	FGameplayAttributeData MaximumStamina;
	ATTRIBUTE_ACCESSORS(UStaminaAttributeSet, MaximumStamina)

	// Holds the current value for Stamina Regeneration.
	UPROPERTY(BlueprintReadOnly, Category = "Stamina Attribute Set", ReplicatedUsing = OnRep_StaminaRegeneration)
	FGameplayAttributeData StaminaRegeneration;
	ATTRIBUTE_ACCESSORS(UStaminaAttributeSet, StaminaRegeneration)

protected:

	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const;

	UFUNCTION()
	virtual void OnRep_CurrentStamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaximumStamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_StaminaRegeneration(const FGameplayAttributeData& OldValue);
};