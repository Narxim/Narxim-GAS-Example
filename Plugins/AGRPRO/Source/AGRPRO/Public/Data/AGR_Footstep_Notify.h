// Copyright Adam Grodzki All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "AGR_Footstep_Notify.generated.h"

UCLASS()
class AGRPRO_API UAGR_Footstep_Notify : public UAnimNotify
{
    GENERATED_BODY()

private:
    const FName DefaultFootSocketName = "DefaultTestAll";

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "AGR")
    FName FootKey = DefaultFootSocketName;

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};