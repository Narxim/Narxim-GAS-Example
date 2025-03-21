// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "CustomRogueLiteTools/AbilitySystem/AbilityTypes.h"
#include "Runtime/Engine/Classes/Components/ActorComponent.h"
#include "CRLActorComponent.generated.h"

USTRUCT(BlueprintType)
struct FCRLModifierDefinition
{
	GENERATED_BODY()
	
	/**
	 * Abilities that will be called if you are the instigator
	 */
	UPROPERTY(BlueprintReadOnly)
	TSet<TObjectPtr<UCRLAbility>> InstigatorAbilities;

	/**
	 * Abilities that will be called if you are the Target
	 */
	UPROPERTY(BlueprintReadOnly)
	TSet<TObjectPtr<UCRLAbility>> TargetAbilities;

	TSet<TObjectPtr<UCRLAbility>>& GetAbilitiesMutable(const ECRLTargetType TargetType)
	{
		switch (TargetType) {
		default:
		case ECRLTargetType::Instigator:
			{
				return InstigatorAbilities;
			}
		case ECRLTargetType::Target:
			{
				return TargetAbilities;
			}
		}
	}
	
	const TSet<TObjectPtr<UCRLAbility>>& GetAbilities(const ECRLTargetType TargetType) const
	{
		switch (TargetType) {
		default:
		case ECRLTargetType::Instigator:
			{
				return InstigatorAbilities;
			}
		case ECRLTargetType::Target:
			{
				return TargetAbilities;
			}
		}
	}
};

class UCRLAbility;
class ICRLAbilityInterface;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CUSTOMROGUELITETOOLS_API UCRLActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCRLActorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	TMap<ECRLModifierEvent, FCRLModifierDefinition> Modifiers;
	
public:
	bool RegisterModifierAbility(UCRLAbility* Ability, const ECRLModifierEvent Event, const ECRLTargetType TargetType = ECRLTargetType::All);
	bool UnregisterModifierAbility(UCRLAbility* Ability, const ECRLModifierEvent Event, const ECRLTargetType TargetType = ECRLTargetType::All);

	const FCRLModifierDefinition* GetModifierDefinitionsForEvent(const ECRLModifierEvent Event) const;
};
