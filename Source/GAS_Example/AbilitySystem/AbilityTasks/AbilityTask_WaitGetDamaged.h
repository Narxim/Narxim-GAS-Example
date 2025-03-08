// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitGetDamaged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FWaitGetDamagedDelegate, float, DamageDone, float, DamageDoneRatio, const FHitResult&, HitResult, const FGameplayEffectSpec&, Spec, const FGameplayTagContainer&, SourceTags);

/**
 * 
 */
UCLASS()
class GAS_EXAMPLE_API UAbilityTask_WaitGetDamaged : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
    FWaitGetDamagedDelegate	OnGetDamagedDelegate;

	virtual void Activate() override;

	UFUNCTION()
	void OnGetDamaged(float DamageDone, float DamageDoneRatio, const FHitResult& HitResult, const FGameplayEffectSpec& Spec, const FGameplayTagContainer& SourceTags);

	/** Wait until the owner of this ability gets damaged. */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
    static UAbilityTask_WaitGetDamaged* WaitForDamage(UGameplayAbility* OwningAbility, bool TriggerOnce=true);

	/** Wait until the owner of this ability gets damaged, with threshold requirements. */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
    static UAbilityTask_WaitGetDamaged* WaitForDamageWithThreshold(UGameplayAbility* OwningAbility, float Threshold, bool TriggerOnce = true);

	/** Wait until the owner of this ability gets damaged, with ratio threshold requirements. */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
    static UAbilityTask_WaitGetDamaged* WaitForDamageWithThresholdRatio(UGameplayAbility* OwningAbility, float ThresholdRatio, bool TriggerOnce=true);

	bool TriggerOnce;
	float Threshold;
	float ThresholdRatio;

protected:
    virtual void OnDestroy(bool AbilityEnded) override;

	FDelegateHandle OnGetDamagedActivateDelegateHandle;

private:
	UFUNCTION()
	void OnReplicatedDamageEvent();
};
