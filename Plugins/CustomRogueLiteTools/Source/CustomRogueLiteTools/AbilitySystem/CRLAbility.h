// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "AbilityTypes.h"
#include "GAS_Example/AbilitySystem/GameplayAbilities/CustomGameplayAbility_ClientToServer.h"
#include "CRLAbility.generated.h"

class UCRLActorComponent;
/**
 * 
 */
UCLASS()
class CUSTOMROGUELITETOOLS_API UCRLAbility : public UCustomGameplayAbility_ClientToServer
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bAutoRegisterAsModifier = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "bAutoRegisterAsModifier", EditConditionHides = "bAutoRegisterAsModifier"))
	ECRLTargetType RegisteredTargetType = ECRLTargetType::Instigator;

	// Result of the registration
	bool bRegisteredAsModifier = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSet<ECRLModifierEvent> RegisteredEvents;
	
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UFUNCTION(BlueprintPure)
	UCRLActorComponent* GetCRLActorComponent() const;

	UFUNCTION(BlueprintPure)
	bool IsOwningActorCRLCompatible() const;
	
public:
	UFUNCTION(BlueprintNativeEvent)
	bool ProcessChangedAttributeCollection(const ECRLTargetType TargetType, UPARAM(ref) FCRLChangedAttributeCollection& Collection);
	virtual bool ProcessChangedAttributeCollection_Implementation(const ECRLTargetType TargetType, UPARAM(ref) FCRLChangedAttributeCollection& Collection);

	UFUNCTION(BlueprintCallable)
	bool RegisterCall(const AActor* SourceActor, ECRLModifierEvent Event, const ECRLTargetType TargetType = ECRLTargetType::All);
	
	UFUNCTION(BlueprintCallable)
	bool Unregister(const AActor* SourceActor, const ECRLTargetType TargetType = ECRLTargetType::All);
};
