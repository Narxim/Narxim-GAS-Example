// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "CustomGameplayEffectApplicationRequirement.generated.h"

struct FGameplayEffectSpec;
class UGameplayEffect;
class UAbilitySystemComponent;

/** Class used to perform custom requirement, either via blueprint or native code */ 
UCLASS(Blueprintable)
class GAS_EXAMPLE_API UCustomGameplayEffectApplicationRequirement : public UGameplayEffectComponent
{

	GENERATED_BODY()

public:
	/** Constructor to set EditorFriendlyName */
	UCustomGameplayEffectApplicationRequirement();
	
	virtual bool CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec) const override;
	
	/** Return whether the gameplay effect should be applied or not */
	UFUNCTION(BlueprintNativeEvent, Category="Calculation", DisplayName="CanApplyGameplayEffect")
	bool K0_CanApplyGameplayEffect(const UGameplayEffect* const GameplayEffect, const FGameplayEffectSpec& Spec, const UAbilitySystemComponent* const ASC) const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
