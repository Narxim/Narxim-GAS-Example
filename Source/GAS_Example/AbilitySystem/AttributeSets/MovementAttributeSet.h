// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSetBase.h"
#include "MovementAttributeSet.generated.h"


// Contains Attributes related to Movement.
//
//	Contents:
UCLASS()
class GAS_EXAMPLE_API UMovementAttributeSet : public UAttributeSetBase
{
	GENERATED_BODY()

public:
	
	UMovementAttributeSet();

	// Attribute Set Overrides.
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual void ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const override;

	// Set Attributes to replicate.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Category = "Movement Attribute Set", ReplicatedUsing = OnRep_MovementSpeedMultiplier)
	FGameplayAttributeData MovementSpeedMultiplier;
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, MovementSpeedMultiplier)

	UPROPERTY(BlueprintReadOnly, Category = "Movement Attribute Set", ReplicatedUsing = OnRep_MovementAccelerationMultiplier)
	FGameplayAttributeData MovementAccelerationMultiplier;
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, MovementAccelerationMultiplier)
protected:
	
	UFUNCTION()
	virtual void OnRep_MovementSpeedMultiplier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MovementAccelerationMultiplier(const FGameplayAttributeData& OldValue);
};