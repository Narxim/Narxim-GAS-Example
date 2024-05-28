// Copyright 2021 Joseph "Narxim" Thigpen.


#include "CustomGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"


UCustomGameplayAbility::UCustomGameplayAbility()
{
	// Sets the ability to default to Instanced Per Actor.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;	
}

void UCustomGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	// Set the "Avatar Character" reference.
	AvatarCharacter = Cast<ACharacter>(ActorInfo->AvatarActor);

	// Set up Bindings for Enhanced Input.
	SetupEnhancedInputBindings(ActorInfo, Spec);

	// Try to Activate immediately if "Activate Ability On Granted" is true.
	if (ActivateAbilityOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
	}
}

void UCustomGameplayAbility::SetupEnhancedInputBindings(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (IsValid(ActorInfo->AvatarActor.Get()))
	{
		if (const APawn* AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor.Get()))
		{
			if (const AController* PawnController = AvatarPawn->GetController())
			{
				if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PawnController->InputComponent.Get()))
				{
					if (UCustomGameplayAbility* AbilityInstance = Cast<UCustomGameplayAbility>(Spec.Ability.Get()))
					{
						// Check to see if the "Activation Input Action" is valid.
						if (IsValid(AbilityInstance->ActivationInputAction))
						{
							// If we have a valid "Input Pressed Trigger" type bind the pressed event.
							if (InputPressedTriggerType != ETriggerEvent::None)
							{
								EnhancedInputComponent->BindAction(AbilityInstance->ActivationInputAction, AbilityInstance->InputPressedTriggerType, AbilityInstance, &ThisClass::HandleInputPressedEvent, ActorInfo, Spec.Handle);
							}

							// If we have a valid "Input Released Trigger" type bind the released event.
							if (InputReleasedTriggerType != ETriggerEvent::None)
							{
								EnhancedInputComponent->BindAction(AbilityInstance->ActivationInputAction, AbilityInstance->InputReleasedTriggerType, AbilityInstance, &ThisClass::HandleInputReleasedEvent, ActorInfo, Spec.Handle);
							}
						}
					}
				}
			}
		}
	}
}

void UCustomGameplayAbility::HandleInputPressedEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle)
{
	// Find the Ability Spec based on the passed in information and set a reference.
	if (FGameplayAbilitySpec* Spec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(SpecHandle))
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get())
		{
			Spec->InputPressed = true;
			
			if (Spec->IsActive())
			{
				if (Spec->Ability.Get()->bReplicateInputDirectly && AbilitySystemComponent->IsOwnerActorAuthoritative() == false)
				{
					AbilitySystemComponent->ServerSetInputPressed(Spec->Ability.Get()->GetCurrentAbilitySpecHandle());
				}

				AbilitySystemComponent->AbilitySpecInputPressed(*Spec);

				// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
				AbilitySystemComponent->InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec->Handle, Spec->ActivationInfo.GetActivationPredictionKey());
			}
			else
			{
				// Ability is not active, so try to activate it
				AbilitySystemComponent->TryActivateAbility(SpecHandle);
			}
		}
	}
}

void UCustomGameplayAbility::HandleInputReleasedEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle)
{
	// Find the Ability Spec based on the passed in information and set a reference.
	if (FGameplayAbilitySpec* Spec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(SpecHandle))
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get())
		{
			Spec->InputPressed = false;
			
			if (Spec && Spec->IsActive())
			{
				if (Spec->Ability.Get()->bReplicateInputDirectly && AbilitySystemComponent->IsOwnerActorAuthoritative() == false)
				{
					AbilitySystemComponent->ServerSetInputReleased(SpecHandle);
				}

				AbilitySystemComponent->AbilitySpecInputReleased(*Spec);

				// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
				AbilitySystemComponent->InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, SpecHandle, Spec->ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
}

void UCustomGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (IsValid(ActorInfo->AvatarActor.Get()))
	{
		if (const APawn* AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor.Get()))
		{
			if (const AController* PawnController = AvatarPawn->GetController())
			{
				if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PawnController->InputComponent.Get()))
				{
					// We need to clear the bindings from the Enhanced Input Component when the Ability is removed.
					EnhancedInputComponent->ClearBindingsForObject(Spec.Ability.Get());
				}
			}
		}
	}
	
	Super::OnRemoveAbility(ActorInfo, Spec);
}