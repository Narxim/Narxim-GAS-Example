// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBindings.h"
#include "Abilities/GameplayAbility.h"
#include "CustomGameplayAbility.generated.h"


// Used to modify the strength of the Ability based off of Attributes.
USTRUCT(BlueprintType)
struct FAbilityModifierStruct
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FGameplayAttribute BackingAttribute;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float AttributeModifier = 1;
};

// Custom Gameplay Ability extends the functionality of its base class.

UCLASS()
class GAS_EXAMPLE_API UCustomGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UCustomGameplayAbility();

	// Which Action Mapping to use for this Ability.
	// For Abilities that do not use action mappings (ex: Passive Abilities) use "None".
	// You can add new Input Bindings in the "GameplayAbilityBindings" C++ class.
	// Note: You will need to set up an Action Mapping in the editor with a matching name to use these bindings.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability Input")
	EGameplayAbilityBindings InputBinding = EGameplayAbilityBindings::None;

	// Tells an ability to activate immediately when its granted. Used for passive abilities and abilities forced on others.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability Input")
	bool ActivateAbilityOnGranted = false;

	// The base strength of this Ability. Can be used for SetByCaller effects or anything float related.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Modifiers")
	FScalableFloat BaseAbilityStrength;

	// The Attribute based modifiers this ability has.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Modifiers")
	TArray<FAbilityModifierStruct> AbilityModifiers;
	
protected:

	// Think of this as "BeginPlay".
	// Add logic here that should run when the Ability is first initialized.
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

public:

	// Applies the Gameplay Effect to the owner of this Ability.
	UFUNCTION(BlueprintCallable, Category = "Custom Gameplay Ability")
	void ApplyGameplayEffectToOwner(TSubclassOf<class UGameplayEffect> EffectToAdd, float EffectLevel);

	// Applies the Gameplay Effect to the supplied Actor.
	UFUNCTION(BlueprintCallable, Category = "Custom Gameplay Ability")
	void ApplyGameplayEffectToTarget(TSubclassOf<class UGameplayEffect> EffectToAdd, AActor* Target, float EffectLevel);

	// Applies the Gameplay Effect with a Set by Caller Value to the Target using the Gameplay Effect class, Effect Level and Set By Caller Tag and Amount.
	UFUNCTION(BlueprintCallable, Category = "Custom Gameplay Ability")
	void ApplySetByCallerGameplayEffect(AActor* Target, TSubclassOf<class UGameplayEffect> GameplayEffect, float EffectLevel, FGameplayTag SetByCallerTag, float SetByCallerAmount);
	
	// Returns the strength of this Ability after all modifiers have been calculated.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Custom Gameplay Ability|Modifiers")
	float GetModifiedAbilityStrength();
};
