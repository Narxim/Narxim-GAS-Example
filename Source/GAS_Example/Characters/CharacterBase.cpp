// Copyright 2021 Joseph "Narxim" Thigpen.


#include "CharacterBase.h"

#include "CustomCharacterMovementComponent.h"
#include "GAS_Example/AbilitySystem/AbilitySystemComponent/CustomAbilitySystemComponent.h"


ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer
		.SetDefaultSubobjectClass<UCustomCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
}

UCustomCharacterMovementComponent* ACharacterBase::GetCustomCharacterMovementComponent() const
{
	return Cast<UCustomCharacterMovementComponent>(GetCharacterMovement());
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UCustomAbilitySystemComponent* ACharacterBase::GetCustomAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}
