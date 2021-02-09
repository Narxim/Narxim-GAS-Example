// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"

// Gameplay Ability Bindings provide the ability to bind GA's to input bindings in Editor.
// These bindings will be provided as an Enum on the "CustomGameplayAbility" class and can be
// bound in editor under Project Settings->Engine->Input->Action Mappings. For passive Abilities
// or Abilities that do not use input, set the binding to "None".

UENUM(BlueprintType)
enum class EGameplayAbilityBindings : uint8
{
	None			UMETA(DisplayName = "None"),
	
	Confirm			UMETA(DisplayName = "Confirm"),

	Cancel			UMETA(DisplayName = "Cancel"),

	Ability1		UMETA(DisplayName = "Ability1"),

	Ability2		UMETA(DisplayName = "Ability2"),

	Ability3		UMETA(DisplayName = "Ability3"),

	Ability4		UMETA(DisplayName = "Ability4"),

	Ability5		UMETA(DisplayName = "Ability5"),

	Sprint			UMETA(DisplayName = "Sprint"),

	Jump			UMETA(DisplayName = "Jump")
};