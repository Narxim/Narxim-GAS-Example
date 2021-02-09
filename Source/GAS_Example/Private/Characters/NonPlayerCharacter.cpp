// Copyright 2021 Joseph "Narxim" Thigpen.


#include "Characters/NonPlayerCharacter.h"
#include "AbilitySystemComponent/CustomAbilitySystemComponent.h"
#include "AttributeSets/HealthAttributeSet.h"
#include "AttributeSets/StaminaAttributeSet.h"


ANonPlayerCharacter::ANonPlayerCharacter()
{
	// Create the Non Player Character Ability System Component sub-object.
	NPC_AbilitySystemComponent = CreateDefaultSubobject<UCustomAbilitySystemComponent>(FName("Ability System Component"));

	// Explicitly set the Ability System Component to replicate.
	NPC_AbilitySystemComponent->SetIsReplicated(true);

	// Sets the Ability System Component to use Minimal replication mode. This will only replicate minimal Gameplay Effect info to connected clients.
	NPC_AbilitySystemComponent->ReplicationMode = EGameplayEffectReplicationMode::Minimal;
	
	// Set the TWeakObjectPtr from Character Base to the newly created Ability System Component sub-object.
	AbilitySystemComponent = NPC_AbilitySystemComponent;
	
	// Create the Health Attribute Set sub-object.
	NPC_HealthAttributes = CreateDefaultSubobject<UHealthAttributeSet>(FName("Health Attributes"));

	// Set the TWeakObjectPtr from Character Base to the newly created Health Attribute Set sub-object.
	HealthAttributes = NPC_HealthAttributes;
	
	// Create the Stamina Attribute Set sub-object.
	NPC_StaminaAttributes = CreateDefaultSubobject<UStaminaAttributeSet>(FName("Stamina Attributes"));

	// Set the TWeakObjectPtr from Character Base to the newly created Stamina Attribute Set sub-object.
	StaminaAttributes = NPC_StaminaAttributes;
}

void ANonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent.IsValid())
	{
		// Set the Owner and Avatar actor for the Ability System Component.
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		GiveDefaultAbilities();

		ApplyDefaultEffects();
	}
}
