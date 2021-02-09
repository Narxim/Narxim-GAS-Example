// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CustomAbilitySystemComponent.generated.h"

//

UCLASS()
class GAS_EXAMPLE_API UCustomAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	UCustomAbilitySystemComponent();

	// Have the owning Character's Default Abilities been given yet?
	bool DefaultAbilitiesGiven = false;

	// Have the owning Character's Default Effects been applied yet?
	bool DefaultEffectsApplied = false;
};