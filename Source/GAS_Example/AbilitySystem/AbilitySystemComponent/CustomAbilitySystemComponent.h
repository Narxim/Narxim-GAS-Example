// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GAS_Example/AbilitySystem/Data/AbilitySystemData.h"
#include "CustomAbilitySystemComponent.generated.h"

class ACharacterBase;

UCLASS()
class GAS_EXAMPLE_API UCustomAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	UCustomAbilitySystemComponent();

	// Called to initialize an Ability System Component with the supplied data. (Can be found in "AbilitySystemData.h")
	// Call this on the Server and Client to properly init references / values.
	UFUNCTION(BlueprintCallable)
	void InitializeAbilitySystemData(const FAbilitySystemInitializationData& InitializationData, AActor* InOwningActor, AActor* InAvatarActor);
	
	// Wrapper for the "GetOrCreateAttributeSubobject" function. Returns the specified Attribute Set / creates one if it isn't found.
	const UAttributeSet* GetOrCreateAttributeSet(const TSubclassOf<UAttributeSet>& InAttributeSet);

	/**
	 * Update all abilities and effects to reach the newly defined level.
	 * @param DesiredLevel New Level as Float. Will be floored to only Complete level will be taken in account
	 * @param bInitialization Should we initialized All attributes, on only the one that are supposed to be leveled up ?
	 */
	UFUNCTION(Reliable, Server, BlueprintCallable)
	virtual void ChangeLevel(const float DesiredLevel, const bool bInitialization = false);

	ACharacterBase* GetCharacterBaseAvatar() const;

protected:
	
	bool AbilitySystemDataInitialized = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSet<FGameplayAttribute> NoLevelAttribute{};
};