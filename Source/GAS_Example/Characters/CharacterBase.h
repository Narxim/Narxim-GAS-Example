// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GAS_Example/AbilitySystem/Data/AbilitySystemData.h"
#include "CharacterBase.generated.h"

// Character Base is a template class that contains all shared logic for "Player Characters" and "Non Player Characters".
// This Class is Abstract and should not be used directly! (Not-Blueprintable)
UCLASS(Abstract, NotBlueprintable)
class GAS_EXAMPLE_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	// Data used to initialize the Ability System Component. (Can be found in "AbilitySystemData.h")
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability System", Meta = (ShowOnlyInnerProperties))
	FAbilitySystemInitializationData AbilitySystemInitializationData;
	
	// Implement the IAbilitySystemInterface. (This is used to find the Ability System Component.)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// This event is fired after Ability System Component initialization is finished.
	UFUNCTION(BlueprintImplementableEvent)
	void PostInitializeAbilitySystem();

protected:

	// Creates a pointer to the Ability System Component associated with this Character.
	// Player Characters will set this in OnRep_PlayerState() locally, and in OnPossessed() server side.
	// Non Player Characters will set this in its constructor.
	UPROPERTY(BlueprintReadOnly)
	UAbilitySystemComponent* AbilitySystemComponent;
};