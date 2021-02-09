// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "PlayerCharacter.generated.h"

// Player Character is the base class to use for any Client controlled Characters.
// This class gets it's Ability System Component and Attribute Sets from the Player State Base class.

UCLASS(Blueprintable)
class GAS_EXAMPLE_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	
protected:

	// Called on the server to acknowledge possession of this Character.
	virtual void PossessedBy(AController* NewController) override;

	// Called on the client when the Character is assigned it's Player State.
	virtual void OnRep_PlayerState() override;

	// Called to bind input to this Character.
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// Called to set-up Ability input bindings for this Character.
	void InitializeAbilityBindings();

	bool AbilityBindingsInitialized = false;
};