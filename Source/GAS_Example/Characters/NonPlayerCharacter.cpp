// Copyright 2021 Joseph "Narxim" Thigpen.


#include "NonPlayerCharacter.h"
#include "GAS_Example/AbilitySystem/AbilitySystemComponent/CustomAbilitySystemComponent.h"


ANonPlayerCharacter::ANonPlayerCharacter()
{
	// Set the pointer from Character Base to the Ability System Component sub-object.
	AbilitySystemComponent = CreateDefaultSubobject<UCustomAbilitySystemComponent>("Ability System Component");
}

void ANonPlayerCharacter::BeginPlay()
{
	if (UCustomAbilitySystemComponent* CustomAbilitySystemComponent = Cast<UCustomAbilitySystemComponent>(AbilitySystemComponent))
	{
		// Call the function on "Custom Ability System Component" to set up references and Init data. (Client and Server)
		CustomAbilitySystemComponent->InitializeAbilitySystemData(AbilitySystemInitializationData, this, this);

		PostInitializeAbilitySystem();
	}

	Super::BeginPlay();
}