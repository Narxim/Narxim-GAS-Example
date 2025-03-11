// Copyright 2021 Joseph "Narxim" Thigpen.

#include "AbilityTask_WaitGetDamaged.h"
#include "AbilitySystemComponent.h"

void UAbilityTask_WaitGetDamaged::Activate()
{
	// Not finished, won't do anything.
}

void UAbilityTask_WaitGetDamaged::OnGetDamaged(float DamageDone, float DamageDoneRatio, const FHitResult& HitResult, const FGameplayEffectSpec& Spec, const FGameplayTagContainer& SourceTags)
{
	if (Threshold > 0.f && DamageDone < Threshold)
	{
		return;
	}

	if (ThresholdRatio > 0.f && DamageDoneRatio < ThresholdRatio)
	{
		return;
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnGetDamagedDelegate.Broadcast(DamageDone, DamageDoneRatio, HitResult, Spec, SourceTags);
	}

	if (TriggerOnce)
	{
		EndTask();
	}
}

UAbilityTask_WaitGetDamaged* UAbilityTask_WaitGetDamaged::WaitForDamage(UGameplayAbility* OwningAbility, bool TriggerOnce/*=true*/)
{
	UAbilityTask_WaitGetDamaged* MyObj = NewAbilityTask<UAbilityTask_WaitGetDamaged>(OwningAbility);
	MyObj->Threshold = 0.f;
	MyObj->ThresholdRatio = 0.f;
	MyObj->TriggerOnce = TriggerOnce;
	return MyObj;
}

UAbilityTask_WaitGetDamaged* UAbilityTask_WaitGetDamaged::WaitForDamageWithThreshold(UGameplayAbility* OwningAbility, float Threshold, bool TriggerOnce /*= true*/)
{
	UAbilityTask_WaitGetDamaged* MyObj = NewAbilityTask<UAbilityTask_WaitGetDamaged>(OwningAbility);
	MyObj->Threshold = Threshold;
	MyObj->ThresholdRatio = 0.f;
	MyObj->TriggerOnce = TriggerOnce;
	return MyObj;
}

UAbilityTask_WaitGetDamaged* UAbilityTask_WaitGetDamaged::WaitForDamageWithThresholdRatio(UGameplayAbility* OwningAbility, float ThresholdRatio, bool TriggerOnce/*=true*/)
{
	UAbilityTask_WaitGetDamaged* MyObj = NewAbilityTask<UAbilityTask_WaitGetDamaged>(OwningAbility);
	MyObj->Threshold = 0.f;
	MyObj->ThresholdRatio = ThresholdRatio;
	MyObj->TriggerOnce = TriggerOnce;
	return MyObj;
}

void UAbilityTask_WaitGetDamaged::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);
}

void UAbilityTask_WaitGetDamaged::OnReplicatedDamageEvent()
{
	OnGetDamagedDelegate.Broadcast(0.f, 0.f, FHitResult(), FGameplayEffectSpec(), FGameplayTagContainer::EmptyContainer);
}
