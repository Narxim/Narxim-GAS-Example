// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "CustomAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class GAS_EXAMPLE_API UCustomAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	virtual FGameplayAbilityActorInfo* AllocAbilityActorInfo() const override;
};
