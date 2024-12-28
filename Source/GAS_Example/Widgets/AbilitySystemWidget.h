// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystemWidget.generated.h"

class UAbilitySystemComponent;

// Contains functions to get values from the Health and Stamina Attribute Sets / bind to their value change delegates.
UCLASS()
class GAS_EXAMPLE_API UAbilitySystemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	// Should this widget bind to Health Attribute Set events.
	// Note: Initialization will fail if the required Attribute Set is not found!
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool ListenForHealthAttributeSetChanges = true;

	// Should this widget bind to Stamina Attribute Set events.
	// Note: Initialization will fail if the required Attribute Set is not found!
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool ListenForStaminaAttributeSetChanges = true;
	
	// Called to initialize the User Widget and bind to Attribute change delegates
	// Can be called again to re-initialize the values
	UFUNCTION(BlueprintCallable, Category = "Ability System")
	bool InitializeAbilitySystemWidget(UAbilitySystemComponent* InOwnerAbilitySystemComponent);

	// Returns the Owner's Ability System Component.  
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAbilitySystemComponent* GetOwnerAbilitySystemComponent() const;

	// Event called when the Current level changes.
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability System")
	void On_LevelChanged(const float NewValue, const float OldValue);
	
	// Event called when the Maximum Health attribute value changes.
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability System")
	void On_MaximumHealthChanged(const float NewValue, const float OldValue, const float NewPercentage);

	// Event called when the Current Health attribute value changes.
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability System")
	void On_CurrentHealthChanged(const float NewValue, const float OldValue, const float NewPercentage);

	// Event called when the Health Regeneration attribute value changes.
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability System")
	void On_HealthRegenerationChanged(const float NewValue, const float OldValue);

	// Event called when the  Maximum Stamina attribute value changes.
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability System")
	void On_MaximumStaminaChanged(const float NewValue, const float OldValue, const float NewPercentage);

	// Event called when the Current Stamina attribute value changes.
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability System")
	void On_CurrentStaminaChanged(const float NewValue, const float OldValue, const float NewPercentage);

	// Event called when the Stamina Regeneration attribute value changes.
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability System")
	void On_StaminaRegenerationChanged(const float NewValue, const float OldValue);

	// Event called when the Armor attribute value changes.
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability System")
	void On_ResistanceChanged(const float NewBase, const float NewMagnitude);

	// Event called when the Bleeding attribute value changes.
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability System")
	void On_BleedingChanged(const float NewValue, const float NewHealPerSecond, const float NewRemainingDuration);
	
protected:

	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	FDelegateHandle MaximumHealthChangeDelegate;
	FDelegateHandle CurrentHealthChangeDelegate;
	FDelegateHandle HealthRegenerationChangeDelegate;
	FDelegateHandle MaximumStaminaChangeDelegate;
	FDelegateHandle CurrentStaminaChangeDelegate;
	FDelegateHandle StaminaRegenerationChangeDelegate;
	FDelegateHandle LevelChangeDelegate;
	FDelegateHandle ResistanceChangeDelegate;
	FDelegateHandle BleedingChangeDelegate;
	FDelegateHandle BleedHealChangeDelegate;

	void CurrentLevelChanged(const FOnAttributeChangeData& Data);
	
	void MaximumHealthChanged(const FOnAttributeChangeData& Data);
	
	void CurrentHealthChanged(const FOnAttributeChangeData& Data);
	
	void HealthRegenerationChanged(const FOnAttributeChangeData& Data);
	
	void ResistanceChanged(const FOnAttributeChangeData& Data);

	void MaximumStaminaChanged(const FOnAttributeChangeData& Data);
	
	void CurrentStaminaChanged(const FOnAttributeChangeData& Data);
	
	void StaminaRegenerationChanged(const FOnAttributeChangeData& Data);
	
	void BleedingChanged(const FOnAttributeChangeData& Data);

	static void ResetDelegateHandle(FDelegateHandle DelegateHandle, UAbilitySystemComponent* OldAbilitySystemComponent, const FGameplayAttribute& Attribute);
};
