// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "CustomGameplayEffect.h"
#include "CooldownGameplayEffectBase.generated.h"

/**
 * 
 */
UCLASS()
class GAS_EXAMPLE_API UCooldownGameplayEffectBase : public UCustomGameplayEffect
{
	GENERATED_BODY()

	UCooldownGameplayEffectBase();

	virtual void PostLoad() override;
	virtual void PostInitProperties() override;
	void ConfigureEffect();
};
