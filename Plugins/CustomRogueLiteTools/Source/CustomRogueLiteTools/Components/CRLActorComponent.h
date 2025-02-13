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

	UPROPERTY(BlueprintReadOnly)
	TSet<TObjectPtr<UCRLAbility>> Abilities;
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

	UPROPERTY()
	TArray<FCRLModifierDefinition> Modifiers1{};
public:

	bool RegisterModifierAbility(UCRLAbility* Ability, ECRLModifierEvent Event);
	bool UnregisterModifierAbility(UCRLAbility* Ability, ECRLModifierEvent Event);

	const FCRLModifierDefinition* GetModifierDefinitionsForEvent(const ECRLModifierEvent Event) const;
};
