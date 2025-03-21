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
	mutable const UCRLActorComponent* SourceCLRComponent = nullptr;
	mutable const UCRLActorComponent* TargetCLRComponent = nullptr;
	mutable FCRLChangedAttributeCollection AttributeCollection{};
	
public:
	UCRLDamageExecution();
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
	void ProcessModifierEvent(ECRLModifierEvent Event) const;

	UFUNCTION(BlueprintPure, Category = "CRL|Damage")
	static bool IsCriticalHitFromAttributeCollection(const FCRLChangedAttributeCollection& AttributeCollection)
	{
		return AttributeCollection.IsCriticalHit();
	}

	UFUNCTION(BlueprintPure, Category = "CRL|Damage")
	static float GetMagnitudeFromAttributeCollection(const FCRLChangedAttributeCollection& AttributeCollection, const FGameplayTag TagIdentifier)
	{
		if (const FCRLChangedValue* const Def = AttributeCollection.ChangedAttributeCollection.Find(TagIdentifier))
		{
			return Def->GetMagnitude();
		}
		return 0.f;
	}

	UFUNCTION(BlueprintPure, Category = "CRL|Damage|Identifiers")
	static FGameplayTag GetIdentifierTag_Damage()
	{
		return CRLTags::TAG_CRL_Identifier_Damage.GetTag();
	}
	UFUNCTION(BlueprintPure, Category = "CRL|Damage|Identifiers")
	static FGameplayTag GetIdentifierTag_Resistance()
	{
		return CRLTags::TAG_CRL_Identifier_Resistance.GetTag();
	}
	UFUNCTION(BlueprintPure, Category = "CRL|Damage|Identifiers")
	static FGameplayTag GetIdentifierTag_Critic()
	{
		return CRLTags::TAG_CRL_Identifier_Critic.GetTag();
	}

	UFUNCTION(BlueprintCallable)
	static bool GetModifierFromAttributeCollection(UPARAM(ref) FCRLChangedAttributeCollection& Collection, const FGameplayTag Identifier, UPARAM(ref) FCRLChangedValue& OutModifier);
};