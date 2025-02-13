// Copyright 2021 Joseph "Narxim" Thigpen.


#include "CRLActorComponent.h"


// Sets default values for this component's properties
UCRLActorComponent::UCRLActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UCRLActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

bool UCRLActorComponent::RegisterModifierAbility(UCRLAbility* Ability, ECRLModifierEvent Event)
{
	FCRLModifierDefinition& Def = Modifiers.FindOrAdd(Event);
	Def.Abilities.Add(Ability);
	return true;
}

bool UCRLActorComponent::UnregisterModifierAbility(UCRLAbility* Ability, ECRLModifierEvent Event)
{
	FCRLModifierDefinition* const Def = Modifiers.Find(Event);
	if (!Def)
	{
		return false;
	}

	return Def->Abilities.Remove(Ability) >= 0;
}

const FCRLModifierDefinition* UCRLActorComponent::GetModifierDefinitionsForEvent(const ECRLModifierEvent Event) const
{
	return Modifiers.Find(Event);
}

