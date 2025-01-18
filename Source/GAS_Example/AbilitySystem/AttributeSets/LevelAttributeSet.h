// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSetBase.h"
#include "LevelAttributeSet.generated.h"


// Contains Attributes related to Level. Any Character that uses Level will need this Attribute Set.
//
//	Contents:
//	
//	CurrentLevel - Primarily used as a cost resource for Abilities.
//	MaximumLevel - The maximum amount of Level the Character can have.
UCLASS()
class GAS_EXAMPLE_API ULevelAttributeSet : public UAttributeSetBase
{
	GENERATED_BODY()

public:
	
	ULevelAttributeSet();

	// Attribute Set Overrides.
	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const override;

	// Set Attributes to replicate.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Holds the current value for Level.
	UPROPERTY(BlueprintReadOnly, Category = "Level Attribute Set", ReplicatedUsing = OnRep_CurrentLevel)
	FGameplayAttributeData CurrentLevel;
	ATTRIBUTE_ACCESSORS(ULevelAttributeSet, CurrentLevel)

	// Holds the value for Maximum Level.
	UPROPERTY(BlueprintReadOnly, Category = "Level Attribute Set", ReplicatedUsing = OnRep_MaximumLevel)
	FGameplayAttributeData MaximumLevel;
	ATTRIBUTE_ACCESSORS(ULevelAttributeSet, MaximumLevel)

protected:
	
	UFUNCTION()
	virtual void OnRep_CurrentLevel(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaximumLevel(const FGameplayAttributeData& OldValue);
};