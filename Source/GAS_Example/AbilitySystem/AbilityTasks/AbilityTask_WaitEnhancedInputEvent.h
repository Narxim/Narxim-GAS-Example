// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitEnhancedInputEvent.generated.h"

class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnhancedInputEventDelegate, FInputActionValue, Value);

// Uses the Avatar Actor from the owning Gameplay Ability to search for its Enhanced Input Component; if found, binds to the delegate for the specified Input Action and Trigger Type.
UCLASS()
class GAS_EXAMPLE_API UAbilityTask_WaitEnhancedInputEvent : public UAbilityTask
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable)
	FEnhancedInputEventDelegate InputEventReceived;
	
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"), Category = "Ability Tasks")
	static UAbilityTask_WaitEnhancedInputEvent* WaitEnhancedInputEvent(UGameplayAbility* OwningAbility, const FName TaskInstanceName, UInputAction* InputAction, const ETriggerEvent TriggerEventType, bool bShouldOnlyTriggerOnce = true);

private:

	TWeakObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = nullptr;
	
	TWeakObjectPtr<UInputAction> InputAction = nullptr;
	
	ETriggerEvent EventType;
	
	bool bTriggerOnce;

	bool bHasBeenTriggered = false;

	virtual void Activate() override;

	void EventReceived(const FInputActionValue& Value);

	virtual void OnDestroy(const bool bInOwnerFinished) override;
};