// Copyright 2021 Joseph "Narxim" Thigpen.


#include "AbilityTask_WaitGetDamaged.h"
#include "AbilitySystemComponent.h"

void UAbilityTask_WaitGetDamaged::Activate()
{
	// if (AbilitySystemComponent.IsValid())
	// {
	// 	if (AIBaseCharacter* BaseChar = Cast<AIBaseCharacter>(AbilitySystemComponent->GetOwner()))
	// 	{
	// 		BaseChar->OnTakeDamage.AddDynamic(this, &UAbilityTask_WaitGetDamaged::OnGetDamaged);
	//
	// 		AbilitySystemComponent->AbilityReplicatedEventDelegate(REPLICATED_DAMAGE_EVENT, 
	// 			GetAbilitySpecHandle(), GetActivationPredictionKey()).AddUObject(this, 
	// 				&UAbilityTask_WaitGetDamaged::OnReplicatedDamageEvent);
	// 	}
	// }
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

		if (IsForRemoteClient())
		{
			// Check to see if there is pending data, this may call the callback we just set
			// AbilitySystemComponent->CallReplicatedEventDelegateIfSet(REPLICATED_DAMAGE_EVENT,
			// 	GetAbilitySpecHandle(), GetActivationPredictionKey());
		}
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
	// if (AbilitySystemComponent.IsValid())
	// {
	// 	if (AIBaseCharacter* BaseChar = Cast<AIBaseCharacter>(AbilitySystemComponent->GetOwner()))
	// 	{
	// 		BaseChar->OnTakeDamage.RemoveAll(this);
	// 	}
	// }

	Super::OnDestroy(AbilityEnded);
}

void UAbilityTask_WaitGetDamaged::OnReplicatedDamageEvent()
{
	OnGetDamagedDelegate.Broadcast(0.f, 0.f, FHitResult(), FGameplayEffectSpec(), FGameplayTagContainer::EmptyContainer);
}
