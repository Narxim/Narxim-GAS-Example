// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GAS_Example/AbilitySystem/Data/AbilitySystemData.h"
#include "CharacterBase.generated.h"

class UCustomCharacterMovementComponent;
class UCustomAbilitySystemComponent;
// Character Base is a template class that contains all shared logic for "Player Characters" and "Non Player Characters".
// This Class is Abstract and should not be used directly! (Not-Blueprintable)
UCLASS(Abstract, NotBlueprintable)
class GAS_EXAMPLE_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACharacterBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category = "MovementComponent")
	UCustomCharacterMovementComponent* GetCustomCharacterMovementComponent() const;
	
	// Implement the IAbilitySystemInterface. (This is used to find the Ability System Component.)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure, Category = "Ability System")
	UCustomAbilitySystemComponent* GetCustomAbilitySystemComponent() const;
	
	void InitializeAbilitySystem();

	// This event is fired after Ability System Component initialization is finished.
	UFUNCTION(BlueprintNativeEvent)
	void PostInitializeAbilitySystem();

	UFUNCTION(BlueprintPure)
	const FAbilitySystemInitializationData& GetAbilitySystemInitializationData() const
	{
		return AbilitySystemInitializationData;
	}
protected:

	// Data used to initialize the Ability System Component. (Can be found in "AbilitySystemData.h")
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability System", Meta = (ShowOnlyInnerProperties))
	FAbilitySystemInitializationData AbilitySystemInitializationData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCustomAbilitySystemComponent> AbilitySystemComponent;

	void MovementSpeedMultiplierChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;
};