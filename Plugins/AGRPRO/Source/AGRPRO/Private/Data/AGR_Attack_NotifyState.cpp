// Copyright Adam Grodzki All Rights Reserved.

#include "Data/AGR_Attack_NotifyState.h"

#include "Components/AGR_CombatManager.h"

void UAGR_Attack_NotifyState::NotifyBegin(
    USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase* Animation,
    const float TotalDuration,
    const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if(MeshComp)
    {
        if(const AActor* Owner = MeshComp->GetOwner())
        {
            UAGR_CombatManager* CombatManager = Owner->FindComponentByClass<UAGR_CombatManager>();
            if(!IsValid(CombatManager))
            {
                // 	Note: We might also want to check if Owner is not a AAnimationEditorPreviewActor actor (notifiers are triggered in persona preview viewport)
                return;
            }

            CombatManager->StartTrace();
        }
    }
}

void UAGR_Attack_NotifyState::NotifyEnd(
    USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    if(MeshComp)
    {
        if(const AActor* Owner = MeshComp->GetOwner())
        {
            UAGR_CombatManager* CombatManager = Owner->FindComponentByClass<UAGR_CombatManager>();
            if(!IsValid(CombatManager))
            {
                // 	Note: We might also want to check if Owner is not a AAnimationEditorPreviewActor actor (notifiers are triggered in persona preview viewport)
                return;
            }

            CombatManager->EndTrace();
        }
    }
}