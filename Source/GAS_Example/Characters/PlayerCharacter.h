// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "PlayerCharacter.generated.h"

// Player Character is the base class to use for any Client controlled Characters.
// This class gets its Ability System Component from the Player State Base class.
UCLASS(Blueprintable)
class GAS_EXAMPLE_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

protected:

	//
	virtual void InitializeAbilitySystem();

	// Convenience function to handle Enhanced Input movement events 
	UFUNCTION(BlueprintCallable)
	void HandleMovementInput(const float InputX, const float InputY);

	// Convenience function to handle Enhanced Input camera events 
	UFUNCTION(BlueprintCallable)
	void HandleCameraInput(const float InputX, const float InputY);

	// Called on the server to acknowledge possession of this Character.
	virtual void PossessedBy(AController* NewController) override;

	// Called on the client when the Character is assigned its Player State.
	virtual void OnRep_PlayerState() override;
};