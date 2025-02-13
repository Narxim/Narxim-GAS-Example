// Copyright 2021 Joseph "Narxim" Thigpen.


#include "CRL_Damage_NotifyState.h"

#include "CustomRogueLiteTools/Components/CRLCombatManager.h"
#include "CustomRogueLiteTools/Interface/CRLActorInterface.h"

void UCRL_Damage_NotifyState::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if(!MeshComp)
	{
		return;
	}

	const auto CRLActor_Source = Cast<ICRLActorInterface>(MeshComp->GetOwner());
	if (!CRLActor_Source)
	{
		return;
	}
	
	UCRLCombatManager* const CombatManager = CRLActor_Source->GetCRLCombatManager();

	if (!IsValid(CombatManager))
	{
		return;
	}
	CombatManager->StartTrace();
}

void UCRL_Damage_NotifyState::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if(!MeshComp)
	{
		return;
	}

	const auto CRLActor_Source = Cast<ICRLActorInterface>(MeshComp->GetOwner());
	if (!CRLActor_Source)
	{
		return;
	}
	
	UCRLCombatManager* const CombatManager = CRLActor_Source->GetCRLCombatManager();
	if (!IsValid(CombatManager))
	{
		return;
	}
	
	CombatManager->EndTrace();
}