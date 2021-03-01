// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AttributeSets/HealthAttributeSet.h"
#include "AttributeSets/StaminaAttributeSet.h"
#include "GameplayAbility/CustomGameplayAbility.h"
#include "CharacterBase.generated.h"

// Character Base is a template class that contains all shared logic for "Player Characters" and "Non Player Characters".
// This Class is Abstract and should not be used directly! (Non-Blueprintable)

UCLASS(Abstract, NotBlueprintable)
class GAS_EXAMPLE_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	ACharacterBase();

	// The default Abilities this Character has access to.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability System")
	TArray<TSubclassOf<class UCustomGameplayAbility>> DefaultAbilities;

	// The default gameplay Effects this Character has when spawned.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability System")
	TArray<TSubclassOf<class UGameplayEffect>> DefaultEffects;

protected:

	// Creates a pointer to the Ability System Component associated with this Character.
	// Player Characters will set this in OnRep_PlayerState() locally, and in OnPossessed() server side.
	// Non Player Characters will set this in its constructor.
	TWeakObjectPtr<class UCustomAbilitySystemComponent> AbilitySystemComponent;

	// Creates a pointer to the Health Attribute Set associated with this Character.
	// Player Characters will set this in OnRep_PlayerState() locally, and in OnPossessed() server side.
	// Non Player Characters will set this in its constructor.
	TWeakObjectPtr<UHealthAttributeSet> HealthAttributes;

	// Creates a pointer to the Stamina Attribute Set associated with this Character.
	// Player Characters will set this in OnRep_PlayerState() locally, and in OnPossessed() server side.
	// Non Player Characters will set this in its constructor.
	TWeakObjectPtr<UStaminaAttributeSet> StaminaAttributes;

	// Implement the IAbilitySystemInterface. (This is used to find the Ability System Component)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Called to give the Character it's Default Abilities from the "Default Abilities" array.
	void GiveDefaultAbilities();

	// Called to apply the Gameplay Effects in the "Default Effects" array.
	void ApplyDefaultEffects();

public:

	// Gives the supplied Ability to this Character.
	// Should be called on the Server!
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Server, Reliable, WithValidation, Category = "Character Base|Ability System")
	void GiveGameplayAbility(TSubclassOf<UCustomGameplayAbility> AbilityToGive);
};
