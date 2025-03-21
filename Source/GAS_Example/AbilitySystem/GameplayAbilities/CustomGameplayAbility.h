// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Abilities/GameplayAbility.h"
#include "CustomGameplayAbility.generated.h"

class ACharacterBase;
// Custom Gameplay Ability extends the functionality of its base class.
UCLASS()
class GAS_EXAMPLE_API UCustomGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UCustomGameplayAbility();
	
	// Tells an ability to activate immediately when it's granted. (Useful for passive abilities and abilities forced on others)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom Gameplay Ability")
	bool bActivateAbilityOnGranted = false;

	// Which Input Action to bind the activation event to.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ActivationInputAction = nullptr;

	// What Input Action trigger type to listen to for input pressed events.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
	ETriggerEvent InputPressedTriggerType = ETriggerEvent::Started;

	// What Input Action trigger type to listen to for input released events.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
	ETriggerEvent InputReleasedTriggerType = ETriggerEvent::Completed;

	// Returns the "Avatar Character" associated with this Gameplay Ability.
	// Will return null if the Avatar Actor does not derive from Character.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ACharacterBase* GetAvatarCharacter() const { return AvatarCharacter.Get(); }

protected:

	// Keep a pointer to "Avatar Character" so we don't have to cast to Character in instanced abilities owned by a Character derived class.
	TObjectPtr<ACharacterBase> AvatarCharacter = nullptr;

	// ---------- MONTAGES -----------
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montage")
	TSoftObjectPtr<UAnimMontage> Montage{};
	
	UPROPERTY()
	TObjectPtr<UAnimMontage> MontageHardPtr{};

	// Called by OnGiveAbility and will async load the montage
	UFUNCTION()
	void OnMontageLoaded();

	// Called when the loaded montage is valid and loaded
	UFUNCTION(BlueprintNativeEvent, DisplayName="OnMontageLoaded")
	void K0_OnMontageLoaded(UAnimMontage* LoadedMontage);

	UFUNCTION(BlueprintPure)
	bool HasMontageReady() const;

	// Change to modify size of root motion or set to 0 to block it entirely
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montage")
	float AnimRootMotionTranslationScale = true;

	// Start the configured montage as soon as the ability is activated
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montage")
	bool bStartMontageOnActivation = true;

	// Don't stop the montage. Will have to do it manually. Experimental
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montage")
	bool bStopMontageWhenAbilityEnds = true;

	// Stop the ability automatically as soon as the montage is completely ended
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montage")
	bool bEndAbilityOnMontageEnd = true;
	
	UFUNCTION(BlueprintNativeEvent)
	UAnimMontage* GetMontageToUse() const;

	UFUNCTION(BlueprintCallable, Category = "Montage")
	bool PlayConfiguredMontage();

	virtual bool PlayMontage(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, UAnimInstance* AnimInstance);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Montage", DisplayName = "Play Montage")
	void K0_PlayMontage(const FGameplayAbilityActivationInfo ActivationInfo, UAnimInstance* AnimInstance);

	UFUNCTION(BlueprintNativeEvent)
	void OnPreMontage(const FGameplayEventData& TriggerEventData);

	UFUNCTION(BlueprintNativeEvent)
	void OnMontageStarted();
	
	void OnMontageEnded(UAnimMontage* MontageEnded, bool bInterrupted);
	
	UFUNCTION(BlueprintNativeEvent, DisplayName = "OnMontageEnded")
	void K0_OnMontageEnded(UAnimMontage* MontageEnded, bool bInterrupted);

	UFUNCTION(BlueprintNativeEvent)
	float GetMontagePlayRate();

	UFUNCTION(BlueprintNativeEvent)
	FName GetMontageSectionToPlay();

	UFUNCTION(BlueprintNativeEvent)
	float GetMontageStartTime();
	
	// Think of this as "BeginPlay".
	// Add logic here that should run when the Ability is first initialized.
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	UFUNCTION(BlueprintNativeEvent, DisplayName="OnGiveAbility")
	void K0_OnGiveAbility(const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilitySpec& Spec);

	// Called to bind Input Pressed and Input Released events to the Avatar Actor's Enhanced Input Component if it is reachable. 
	void SetupEnhancedInputBindings(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec);

	// Called when the "Activation Input Action" is triggered.
	void HandleInputPressedEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle);

	// Called when the "Activation Input Action" is completed.
	void HandleInputReleasedEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle);

	// Override "OnRemoveAbility" to clean up Enhanced Input Bindings.
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UFUNCTION(BlueprintNativeEvent, DisplayName="OnRemoveAbility")
	void K0_OnRemoveAbility(const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilitySpec& Spec);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	void StopMontage();
};