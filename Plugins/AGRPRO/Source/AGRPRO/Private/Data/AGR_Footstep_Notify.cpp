// Copyright Adam Grodzki All Rights Reserved.

#include "Data/AGR_Footstep_Notify.h"

#include "Components/AGR_SoundMaster.h"

void UAGR_Footstep_Notify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if(!IsValid(MeshComp->GetOwner()))
    {
        return;
    }
    // Don't call super to avoid unnecessary call into blueprints
    UAGR_SoundMaster* SoundComponent = MeshComp->GetOwner()->FindComponentByClass<UAGR_SoundMaster>();
    if(!IsValid(SoundComponent))
    {
        return;
    }

    if(!SoundComponent->bAutoPlace)
    {
        if(FootKey == DefaultFootSocketName)
        {
            SoundComponent->TestAllFeetForCollision();
        }
        else
        {
            SoundComponent->TryTraceFootstep(FootKey);
        }
    }
}