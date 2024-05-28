// Copyright 2021 Joseph "Narxim" Thigpen.


#include "AbilityTask_OnTickEvent.h"


UAbilityTask_OnTickEvent::UAbilityTask_OnTickEvent()
{
	bTickingTask = true;
	bSimulatedTask = true;
}

UAbilityTask_OnTickEvent* UAbilityTask_OnTickEvent::OnTickEvent(UGameplayAbility* OwningAbility, const FName TaskInstanceName)
{
	return NewAbilityTask<UAbilityTask_OnTickEvent>(OwningAbility, TaskInstanceName);
}

void UAbilityTask_OnTickEvent::TickTask(const float DeltaTime)
{
	Super::TickTask(DeltaTime);

	EventReceived(DeltaTime);
}

void UAbilityTask_OnTickEvent::EventReceived(const float DeltaTime) const
{
	if (TickEventReceived.IsBound())
	{
		TickEventReceived.Broadcast(DeltaTime);
	}
}

void UAbilityTask_OnTickEvent::OnDestroy(const bool bInOwnerFinished)
{
	bTickingTask = false;
	
	Super::OnDestroy(bInOwnerFinished);
}