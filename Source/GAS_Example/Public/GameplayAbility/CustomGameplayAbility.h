// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBindings.h"
#include "Abilities/GameplayAbility.h"
#include "CustomGameplayAbility.generated.h"

// Custom Gameplay Ability extends the functionality of its base class.

UCLASS()
class GAS_EXAMPLE_API UCustomGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	// Which Action Mapping to use for this Ability.
	// For Abilities that do not use action mappings (ex: Passive Abilities) use "None".
	// You can add new Input Bindings in the "GameplayAbilityBindings" C++ class.
	// Note: You will need to set up an Action Mapping in the editor with a matching name to use these bindings.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability Input")
	EGameplayAbilityBindings InputBinding = EGameplayAbilityBindings::None;
	
};