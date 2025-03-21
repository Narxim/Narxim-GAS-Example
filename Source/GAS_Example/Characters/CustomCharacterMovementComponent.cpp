// Copyright 2021 Joseph "Narxim" Thigpen.

#include "CustomCharacterMovementComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS_Example/AbilitySystem/AttributeSets/MovementAttributeSet.h"

// Sets default values for this component's properties
UCustomCharacterMovementComponent::UCustomCharacterMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

float UCustomCharacterMovementComponent::GetMaxSpeed() const
{
	return Super::GetMaxSpeed() * MovementSpeedMultiplier;
}

float UCustomCharacterMovementComponent::GetMaxAcceleration() const
{
	return Super::GetMaxAcceleration();
}

void UCustomCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}
