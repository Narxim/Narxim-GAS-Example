// Copyright 2021 Joseph "Narxim" Thigpen.

#include "CRLAbility.h"
#include "CustomRogueLiteTools/Components/CRLActorComponent.h"
#include "CustomRogueLiteTools/Interface/CRLActorInterface.h"

void UCRLAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (!ActorInfo->AvatarActor.IsValid() || !ActorInfo->AvatarActor.Get()->HasAuthority())
	{
		return;
	}
	
	if (bAutoRegisterAsModifier && !RegisteredEvents.IsEmpty())
	{
		for (const ECRLModifierEvent& RegisteredEvent : RegisteredEvents)
		{
			RegisterCall(ActorInfo->AvatarActor.Get(), RegisteredEvent);
		}
	}
}

void UCRLAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (bRegisteredAsModifier)
	{
		if (ActorInfo->AvatarActor.IsValid())
		{
			Unregister(ActorInfo->AvatarActor.Get(), ECRLTargetType::All);
		}
	}
	Super::OnRemoveAbility(ActorInfo, Spec);
}

UCRLActorComponent* UCRLAbility::GetCRLActorComponent() const
{
	if (!IsOwningActorCRLCompatible())
	{
		return nullptr;
	}

	return ICRLActorInterface::Execute_GetCRLActorComponent(GetAvatarActorFromActorInfo());
}

bool UCRLAbility::IsOwningActorCRLCompatible() const
{
	AActor* const Avatar = GetAvatarActorFromActorInfo();
	return !(!Avatar || !Avatar->Implements<UCRLActorInterface>());
}

bool UCRLAbility::ProcessChangedAttributeCollection_Implementation(const ECRLTargetType TargetType, UPARAM(ref) FCRLChangedAttributeCollection& Collection)
{
	return true;
}

bool UCRLAbility::RegisterCall(const AActor* SourceActor, const ECRLModifierEvent Event, const ECRLTargetType TargetType)
{
	if (!SourceActor)
	{
		return false;
	}
	
	if (!SourceActor->Implements<UCRLActorInterface>())
	{
		UE_LOG(LogActor, Error, TEXT("UCRLAbility %s | Register Call | Actor %s doesn't implement ICRLActorInterface"), *this->GetName(), *SourceActor->GetName());
		return false;
	}
	
	UCRLActorComponent* const CRLComponent = ICRLActorInterface::Execute_GetCRLActorComponent(SourceActor);
	if (!CRLComponent)
	{
		return false;
	}
	
	bRegisteredAsModifier |= CRLComponent->RegisterModifierAbility(this, Event, TargetType);
	
	return bRegisteredAsModifier;
}

bool UCRLAbility::Unregister(const AActor* SourceActor, const ECRLTargetType TargetType)
{
	if (!bRegisteredAsModifier)
	{
		return false;
	}
	
	UCRLActorComponent* const CRLComponent = ICRLActorInterface::Execute_GetCRLActorComponent(SourceActor);
	if (!CRLComponent)
	{
		return false;
	}

	for (const ECRLModifierEvent& RegisteredEvent : RegisteredEvents)
	{
		CRLComponent->UnregisterModifierAbility(this, RegisteredEvent, TargetType);
	}

	return true;
}
