// Copyright 2021 Joseph "Narxim" Thigpen.

#include "CRLAbility.h"
#include "CustomRogueLiteTools/Components/CRLActorComponent.h"
#include "CustomRogueLiteTools/Interface/CRLActorInterface.h"

void UCRLAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (bAutoRegisterAsModifier && ActorInfo->AvatarActor.IsValid() && !RegisteredEvents.IsEmpty())
	{
		for (const ECRLModifierEvent& RegisteredEvent : RegisteredEvents)
		{
			bRegisteredAsModifier |= RegisterCall(ActorInfo->AvatarActor.Get(), RegisteredEvent);
		}
	}
}

void UCRLAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (bRegisteredAsModifier)
	{
		if (ActorInfo->AvatarActor.IsValid())
		{
			Unregister(ActorInfo->AvatarActor.Get());
		}
	}
	Super::OnRemoveAbility(ActorInfo, Spec);
}

bool UCRLAbility::ProcessChangedAttributeCollection_Implementation(UPARAM(ref) FCRLChangedAttributeCollection& Collection)
{
	return true;
}

bool UCRLAbility::RegisterCall(const AActor* SourceActor, const ECRLModifierEvent Event)
{
	UCRLActorComponent* const CRLComponent = ICRLActorInterface::Execute_GetCRLActorComponent(SourceActor);
	if (!CRLComponent)
	{
		return false;
	}
	
	CRLComponent->RegisterModifierAbility(this, Event);
	return true;
}

bool UCRLAbility::Unregister(const AActor* SourceActor)
{
	const auto CRLActor_Source = Cast<ICRLActorInterface>(SourceActor);
	
	UCRLActorComponent* const CRLComponent = ICRLActorInterface::Execute_GetCRLActorComponent(SourceActor);
	if (!CRLComponent)
	{
		return false;
	}

	for (const ECRLModifierEvent& RegisteredEvent : RegisteredEvents)
	{
		CRLComponent->UnregisterModifierAbility(this, RegisteredEvent);
	}

	return true;
}
