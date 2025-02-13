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

void UCRLAbility::ProcessChangedAttributeCollection_Implementation(UPARAM(ref) FCRLChangedAttributeCollection& Collection)
{
}

bool UCRLAbility::RegisterCall(const AActor* SourceActor, const ECRLModifierEvent Event)
{
	const auto CRLActor_Source = Cast<ICRLActorInterface>(SourceActor);
	
	UCRLActorComponent* const CRLActorComponent = CRLActor_Source ? CRLActor_Source->GetCRLActorComponent() : nullptr;
	if (!CRLActorComponent)
	{
		return false;
	}

	if (!CRLActorComponent->RegisterModifierAbility(this, Event))
	{
		return false;
	}
	
	RegisteredEvents.Add(Event);
	return true;
}

bool UCRLAbility::Unregister(const AActor* SourceActor)
{
	const auto CRLActor_Source = Cast<ICRLActorInterface>(SourceActor);
	
	UCRLActorComponent* const CRLActorComponent = CRLActor_Source ? CRLActor_Source->GetCRLActorComponent() : nullptr;
	if (!CRLActorComponent)
	{
		return false;
	}

	for (const ECRLModifierEvent& RegisteredEvent : RegisteredEvents)
	{
		CRLActorComponent->UnregisterModifierAbility(this, RegisteredEvent);
	}

	return true;
}
