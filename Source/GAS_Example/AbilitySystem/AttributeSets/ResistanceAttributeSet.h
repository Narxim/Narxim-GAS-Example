// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSetBase.h"
#include "ResistanceAttributeSet.generated.h"


// Contains Attributes related to Resistance. Any Character that uses Resistance will need this Attribute Set.
//
//	Contents:
//	Resistance - Primarily used as reduction / increase to damage or status
UCLASS()
class GAS_EXAMPLE_API UResistanceAttributeSet : public UAttributeSetBase
{
	GENERATED_BODY()

public:
	
	UResistanceAttributeSet();

	static float RESISTANCE_BASE;
	static float RESISTANCE_MAX;
	static float RESISTANCE_MIN;

	// Attribute Set Overrides.
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue);
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const override;

	// Set Attributes to replicate.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Holds the current value for Resistance.
	UPROPERTY(BlueprintReadOnly, Category = "Resistance Attribute Set", ReplicatedUsing = OnRep_Resistance)
	FGameplayAttributeData Resistance;
	ATTRIBUTE_ACCESSORS(UResistanceAttributeSet, Resistance)

	UFUNCTION(BlueprintPure, Category = "Resistance Attribute Set")
	static float GetBaseResistance() { return RESISTANCE_BASE; }

	UFUNCTION(BlueprintPure, Category = "Resistance Attribute Set")
	static FVector2D GetMinMaxResistance() { return FVector2D(RESISTANCE_MIN, RESISTANCE_MAX); }

protected:
	
	UFUNCTION()
	virtual void OnRep_Resistance(const FGameplayAttributeData& OldValue);
};