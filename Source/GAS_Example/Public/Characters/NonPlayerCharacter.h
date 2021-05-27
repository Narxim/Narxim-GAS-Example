// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "NonPlayerCharacter.generated.h"

// Non Player Character is the base class to use for any Server controlled Characters.
// This class contains it's own Ability System Component and Attribute Sets.

UCLASS(Blueprintable)
class GAS_EXAMPLE_API ANonPlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	ANonPlayerCharacter();

	UPROPERTY(BlueprintReadOnly, Category = "Ability System")
	class UCustomAbilitySystemComponent* NPC_AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Ability System")
	class UHealthAttributeSet* NPC_HealthAttributes;

	UPROPERTY(BlueprintReadOnly, Category = "Ability System")
	class UStaminaAttributeSet* NPC_StaminaAttributes;

protected:

	virtual void BeginPlay() override;
};