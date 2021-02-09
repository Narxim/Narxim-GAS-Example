// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent/CustomAbilitySystemComponent.h"
#include "PlayerStateBase.generated.h"

// This version of Player State has an Ability System Component and should be the base of any Player State sub-class when using GAS.


UCLASS()
class GAS_EXAMPLE_API APlayerStateBase : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	APlayerStateBase();

	// The Ability System Component for this PlayerState. 
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability System")
	class UCustomAbilitySystemComponent* AbilitySystemComponent;

	// Implement the IAbilitySystemInterface. (This is used to find the Ability System Component)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// The Health Attribute Set on this Player State.
	UPROPERTY(BlueprintReadOnly, Category = "Ability System")
	class UHealthAttributeSet* HealthAttributes;

	// The Stamina Attribute Set on this Player State.
	UPROPERTY(BlueprintReadOnly, Category = "Ability System")
	class UStaminaAttributeSet* StaminaAttributes;
};