// Copyright 2021 Joseph "Narxim" Thigpen.


#include "PlayerCharacter.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/PlayerState.h"
#include "GAS_Example/AbilitySystem/AbilitySystemComponent/CustomAbilitySystemComponent.h"


void APlayerCharacter::InitializeAbilitySystem()
{
	// Set the Ability System Component pointer using Ability System Globals (IAbilitySystemInterface) with the Player State as a reference.
	AbilitySystemComponent =  Cast<UCustomAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPlayerState()));

	if (!AbilitySystemComponent)
	{
		// Shouldn't happen, but if it is, return an error.
		return;
	}
	
	// Call the function on "Custom Ability System Component" to set up references and Init data. (Client)
	AbilitySystemComponent->InitializeAbilitySystemData(AbilitySystemInitializationData, this, this);

	PostInitializeAbilitySystem();
}

void APlayerCharacter::HandleMovementInput(const float InputX, const float InputY)
{
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardVector, InputY);
	AddMovementInput(RightVector, InputX);
}

void APlayerCharacter::HandleCameraInput(const float InputX, const float InputY)
{
	AddControllerPitchInput(InputY);
	AddControllerYawInput(InputX);
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Server side
	InitializeAbilitySystem();
}

void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Client side
	InitializeAbilitySystem();
}