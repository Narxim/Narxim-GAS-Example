// Copyright 2021 Joseph "Narxim" Thigpen.

#include "CRLPlayerState.h"
#include "CustomRogueLiteTools/AbilitySystem/CRLAbilitySystemComponent.h"

ACRLPlayerState::ACRLPlayerState():Super()
{
	// // If the NetUpdateFrequency is too low, there will be a delay on Ability activation / Effect application on the client.
 //    NetUpdateFrequency = 100.0f;
 //
 //    // Create the Ability System Component sub-object.
 //    AbilitySystemComponent = CreateDefaultSubobject<UCRLAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
 //    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}
