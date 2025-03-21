// Copyright 2021 Joseph "Narxim" Thigpen.


#include "NonPlayerCharacter.h"

ANonPlayerCharacter::ANonPlayerCharacter(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void ANonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!IsRunningDedicatedServer())
	{
		InitializeAbilitySystem();
	}
}
