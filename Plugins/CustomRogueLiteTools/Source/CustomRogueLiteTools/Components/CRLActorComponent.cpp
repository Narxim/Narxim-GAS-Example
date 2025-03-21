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

bool UCRLActorComponent::RegisterModifierAbility(UCRLAbility* Ability, const ECRLModifierEvent Event, const ECRLTargetType TargetType)
{
	FCRLModifierDefinition& Def = Modifiers.FindOrAdd(Event);
	
	switch (TargetType) {
		case ECRLTargetType::Instigator:
			{
				Def.InstigatorAbilities.Add(Ability);
				break;
			}
		case ECRLTargetType::Target:
			{
				Def.TargetAbilities.Add(Ability);
				break;
			}
		default:
		case ECRLTargetType::All:
			{
				Def.TargetAbilities.Add(Ability);
				Def.InstigatorAbilities.Add(Ability);
				break;
			}
	}

	return true;
}

bool UCRLActorComponent::UnregisterModifierAbility(UCRLAbility* Ability, const ECRLModifierEvent Event, const ECRLTargetType TargetType)
{
	FCRLModifierDefinition* const Def = Modifiers.Find(Event);
	if (!Def)
	{
		return false;
	}

	switch (TargetType) {
	case ECRLTargetType::Instigator:
		{
			Def->InstigatorAbilities.Remove(Ability);
			break;
		}
	case ECRLTargetType::Target:
		{
			Def->TargetAbilities.Remove(Ability);
			break;
		}
	default:
	case ECRLTargetType::All:
		{
			Def->TargetAbilities.Remove(Ability);
			Def->InstigatorAbilities.Remove(Ability);
			break;
		}
	}

	return true;
}

const FCRLModifierDefinition* UCRLActorComponent::GetModifierDefinitionsForEvent(const ECRLModifierEvent Event) const
{
	return Modifiers.Find(Event);
}

