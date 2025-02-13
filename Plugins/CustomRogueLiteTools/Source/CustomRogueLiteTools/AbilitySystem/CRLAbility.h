// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "AbilityTypes.h"
#include "GAS_Example/AbilitySystem/GameplayAbilities/CustomGameplayAbility.h"
#include "CRLAbility.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMROGUELITETOOLS_API UCRLAbility : public UCustomGameplayAbility
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bAutoRegisterAsModifier = false;
	
	bool bRegisteredAsModifier = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSet<ECRLModifierEvent> RegisteredEvents;
	
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
public:
	UFUNCTION(BlueprintNativeEvent)
	void ProcessChangedAttributeCollection(UPARAM(ref) FCRLChangedAttributeCollection& Collection);
	virtual void ProcessChangedAttributeCollection_Implementation(UPARAM(ref) FCRLChangedAttributeCollection& Collection);

	bool RegisterCall(const AActor* SourceActor, ECRLModifierEvent Event);
	bool Unregister(const AActor* SourceActor);
};
