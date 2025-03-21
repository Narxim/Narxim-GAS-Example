// Copyright 2021 Joseph "Narxim" Thigpen.


#include "CharacterBase.h"

#include "AbilitySystemGlobals.h"
#include "CustomCharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GAS_Example/AbilitySystem/AbilitySystemComponent/CustomAbilitySystemComponent.h"
#include "GAS_Example/AbilitySystem/AttributeSets/MovementAttributeSet.h"


ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer
.SetDefaultSubobjectClass<UCustomCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// If the NetUpdateFrequency is too low, there will be a delay on Ability activation / Effect application on the client.
	SetNetUpdateFrequency(100.0f);

	// Create the Ability System Component sub-object.
	AbilitySystemComponent = CreateDefaultSubobject<UCustomAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

UCustomCharacterMovementComponent* ACharacterBase::GetCustomCharacterMovementComponent() const
{
	return Cast<UCustomCharacterMovementComponent>(GetCharacterMovement());
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

UCustomAbilitySystemComponent* ACharacterBase::GetCustomAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void ACharacterBase::InitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		// Shouldn't happen, but if it is, return an error.
		return;
	}
	
	// Call the function on "Custom Ability System Component" to set up references and Init data. (Client)
	AbilitySystemComponent->InitializeAbilitySystemData(AbilitySystemInitializationData, this, this);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UMovementAttributeSet::GetMovementSpeedMultiplierAttribute()).AddUObject(this, &ThisClass::MovementSpeedMultiplierChanged);
	
	PostInitializeAbilitySystem();
}

void ACharacterBase::PostInitializeAbilitySystem_Implementation()
{
	if (!AbilitySystemComponent)
	{
		return;
	}
}

void ACharacterBase::MovementSpeedMultiplierChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	UE_LOG(LogTemp, Warning, TEXT("%f"), OnAttributeChangeData.NewValue);
	GetCustomCharacterMovementComponent()->MovementSpeedMultiplier = OnAttributeChangeData.NewValue;
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitializeAbilitySystem();
}

void ACharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();
	InitializeAbilitySystem();
}
