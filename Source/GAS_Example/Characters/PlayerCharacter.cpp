// Copyright 2021 Joseph "Narxim" Thigpen.


#include "PlayerCharacter.h"

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
	InitializeAbilitySystem();
}