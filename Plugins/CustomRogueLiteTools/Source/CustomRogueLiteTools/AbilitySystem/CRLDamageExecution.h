// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "DamageStatics.h"
#include "GameplayEffectExecutionCalculation.h"
#include "CustomRogueLiteTools/Components/CRLActorComponent.h"
#include "GAS_Example/AbilitySystem/AbilitySystemComponent/CustomAbilitySystemComponent.h"

#include "CRLDamageExecution.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMROGUELITETOOLS_API UCRLDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
	static const FCRLDamageStatics* DamageStatics;

protected:
	mutable const UCRLActorComponent* CRLComponent = nullptr;
	mutable FCRLChangedAttributeCollection AttributeCollection{};
	
public:
	UCRLDamageExecution();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	void ProcessModifierEvent(ECRLModifierEvent Event) const;
};
