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

	AActor* const Owner = GetOwner();
	if (!Owner || Owner->GetLocalRole() <= ROLE_SimulatedProxy)
	{
		// Don't do it for Simulated proxies as their movement speed are precalculated and sent by server.
		return;
	}
	
	UAbilitySystemComponent* const ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
	
	if (!ASC || !ASC->HasAttributeSetForAttribute(UMovementAttributeSet::GetMovementSpeedMultiplierAttribute()))
	{
		return;
	}
	
	MovemementSpeedChangeDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(UMovementAttributeSet::GetMovementSpeedMultiplierAttribute()).AddUObject(this, &UCustomCharacterMovementComponent::MovementSpeedMultiplierChanged);
	MovementSpeedMultiplier = ASC->GetNumericAttribute(UMovementAttributeSet::GetMovementSpeedMultiplierAttribute());
}

void UCustomCharacterMovementComponent::MovementSpeedMultiplierChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	MovementSpeedMultiplier = OnAttributeChangeData.NewValue;
}
