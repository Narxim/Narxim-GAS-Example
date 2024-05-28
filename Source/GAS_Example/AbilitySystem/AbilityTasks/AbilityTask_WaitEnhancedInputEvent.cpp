// Copyright 2021 Joseph "Narxim" Thigpen.


#include "AbilityTask_WaitEnhancedInputEvent.h"


UAbilityTask_WaitEnhancedInputEvent* UAbilityTask_WaitEnhancedInputEvent::WaitEnhancedInputEvent(UGameplayAbility* OwningAbility, const FName TaskInstanceName, UInputAction* InputAction, const ETriggerEvent TriggerEventType, const bool bShouldOnlyTriggerOnce)
{
	UAbilityTask_WaitEnhancedInputEvent* AbilityTask = NewAbilityTask<UAbilityTask_WaitEnhancedInputEvent>(OwningAbility, TaskInstanceName);
	
	AbilityTask->InputAction = InputAction;
	AbilityTask->EventType = TriggerEventType;
	AbilityTask->bTriggerOnce = bShouldOnlyTriggerOnce;
	
	return AbilityTask;
}

void UAbilityTask_WaitEnhancedInputEvent::Activate()
{
	Super::Activate();

	if (!AbilitySystemComponent.Get() || !Ability || !InputAction.IsValid())
	{
		return;
	}

	if (const APawn* AvatarPawn = Cast<APawn>(Ability->GetAvatarActorFromActorInfo()))
	{
		if (const APlayerController* PlayerController = Cast<APlayerController>(AvatarPawn->GetController()))
		{
			EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
			
			if (IsValid(EnhancedInputComponent.Get()))
			{
				EnhancedInputComponent->BindAction(InputAction.Get(), EventType, this, &UAbilityTask_WaitEnhancedInputEvent::EventReceived);
			}
		}
	}
}

void UAbilityTask_WaitEnhancedInputEvent::EventReceived(const FInputActionValue& Value)
{
	if (bTriggerOnce && bHasBeenTriggered)
	{
		return;
	}

	bHasBeenTriggered = true;
	
	InputEventReceived.Broadcast(Value);
}

void UAbilityTask_WaitEnhancedInputEvent::OnDestroy(const bool bInOwnerFinished)
{
	if (IsValid(EnhancedInputComponent.Get()))
	{
		EnhancedInputComponent->ClearBindingsForObject(this);
	}
	
	Super::OnDestroy(bInOwnerFinished);
}