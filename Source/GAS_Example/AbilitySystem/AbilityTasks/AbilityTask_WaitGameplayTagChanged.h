// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTagBase.h"
#include "AbilityTask_WaitGameplayTagChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWaitGameplayTagCountDelegate, int32, TagCount, int32, Delta);
/**
 * 
 */
UCLASS()
class GAS_EXAMPLE_API UAbilityTask_WaitGameplayTagChanged : public UAbilityTask_WaitGameplayTag
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FWaitGameplayTagCountDelegate Removed;

	UPROPERTY(BlueprintAssignable)
	FWaitGameplayTagCountDelegate Added;

	bool bTriggerAtActivation = false;

	int32 PreviousCount = 0;

	virtual void Activate() override;
	
	/**
	 * 	Wait until the specified gameplay tag is Added or Removed. By default this will look at the owner of this ability. OptionalExternalTarget can be set to make this look at another actor's tags for changes. 
	 *  If bTriggerAtActivation is not set and it will immediately broadcast either Added or Removed event.
	 *  It will keep listening as long as OnlyTriggerOnce = false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_WaitGameplayTagChanged* WaitGameplayTagChanged(UGameplayAbility* OwningAbility, FGameplayTag Tag, AActor* InOptionalExternalTarget = nullptr, bool bOnlyTriggerOnce = false, bool bTriggerAtActivation = false);

	virtual void GameplayTagCallback(const FGameplayTag InTag, int32 NewCount) override;
};
