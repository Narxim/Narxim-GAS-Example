// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_OnTickEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTickEventDelegate, float, DeltaTime);

// Fires an event on tick!
UCLASS()
class GAS_EXAMPLE_API UAbilityTask_OnTickEvent : public UAbilityTask
{
	GENERATED_BODY()

public:

	UAbilityTask_OnTickEvent();
	
	UPROPERTY(BlueprintAssignable)
	FTickEventDelegate TickEventReceived;
	
	UFUNCTION(BlueprintCallable, Meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "True"), Category = "Ability Tasks")
	static UAbilityTask_OnTickEvent* OnTickEvent(UGameplayAbility* OwningAbility, const FName TaskInstanceName);

protected:

	virtual void TickTask(const float DeltaTime) override;
	
	void EventReceived(const float DeltaTime) const;

	virtual void OnDestroy(const bool bInOwnerFinished) override;
};