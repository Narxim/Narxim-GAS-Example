// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "HealthAttributeSet.generated.h"

// Use macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// Contains Attributes related to Health. Any Character that takes damage will need this Attribute Set.
//
// Contents:
//
// Current Health - How much Health the Character has at this moment.
// Maximum Health - The maximum amount of Health the Character can have.
// Health Regeneration - How much Health is gained per period of Regeneration.
//
// Damage - A meta Attribute that negatively changes the value of Health.
// Healing - A meta Attribute that positively changes the value of Health.

UCLASS()
class GAS_EXAMPLE_API UHealthAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	
	UHealthAttributeSet();

	// Attribute Set Overrides.
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// Set Attributes to replicate.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Used to create a local copy of Damage which is then subtracted from Current Health.
	UPROPERTY(BlueprintReadOnly, Category = "Health Attribute Set", meta = (HideFromLevelInfos))
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Damage)

	// Used to create a local copy of Healing which is then added to Current Health.
	UPROPERTY(BlueprintReadOnly, Category = "Health Attribute Set", meta = (HideFromLevelInfos))
	FGameplayAttributeData Healing;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Healing)
	
	// Holds the current value for Health.
	UPROPERTY(BlueprintReadOnly, Category = "Health Attribute Set", ReplicatedUsing = OnRep_CurrentHealth)
	FGameplayAttributeData CurrentHealth;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, CurrentHealth)

	// Holds the value for Maximum Health.
	UPROPERTY(BlueprintReadOnly, Category = "Health Attribute Set", ReplicatedUsing = OnRep_MaximumHealth)
	FGameplayAttributeData MaximumHealth;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaximumHealth)

	// Holds the value for Health Regeneration.
	UPROPERTY(BlueprintReadOnly, Category = "Health Attribute Set", ReplicatedUsing = OnRep_HealthRegeneration)
	FGameplayAttributeData HealthRegeneration;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, HealthRegeneration)

protected:

	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const;

	UFUNCTION()
	virtual void OnRep_CurrentHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaximumHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_HealthRegeneration(const FGameplayAttributeData& OldValue);
};