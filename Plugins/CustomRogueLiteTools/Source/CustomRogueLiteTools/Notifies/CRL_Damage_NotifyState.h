// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "Data/AGR_Attack_NotifyState.h"
#include "CRL_Damage_NotifyState.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMROGUELITETOOLS_API UCRL_Damage_NotifyState : public UAGR_Attack_NotifyState
{
	GENERATED_BODY()


	void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	                 const FAnimNotifyEventReference& EventReference);
	void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	               const FAnimNotifyEventReference& EventReference);
};
