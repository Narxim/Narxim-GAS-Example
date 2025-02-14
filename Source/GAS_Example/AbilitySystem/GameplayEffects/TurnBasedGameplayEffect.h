// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "TurnBasedGameplayEffect.generated.h"

/**
 * Simplifies the creation of infinite Gameplay Effects for designers.  This class automatically sets the GE duration
 * to infinite and adds/manages the required `TurnBasedGameplayEffectComponent`.
 */
UCLASS()
class GAS_EXAMPLE_API UTurnBasedGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UTurnBasedGameplayEffect();

	virtual void PostInitProperties() override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR
	
};
