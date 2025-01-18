// Copyright Adam Grodzki All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundCue.h"

#include "DA_AGR_FootstepConfig.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AGRPRO_API UDA_AGR_FootstepConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGR|Setup")
    float MinVelocity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGR|Setup")
    float MaxVelocity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGR|Setup")
    TMap<TEnumAsByte<EPhysicalSurface>, USoundCue*> Sounds = {
        {EPhysicalSurface::SurfaceType_Default, nullptr},
    };

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGR|Setup")
    TMap<TEnumAsByte<EPhysicalSurface>, UNiagaraSystem*> Particles = {
        {EPhysicalSurface::SurfaceType_Default, nullptr},
    };

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGR|Setup")
    USoundAttenuation* Attenuation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGR|Setup")
    USoundConcurrency* Concurrency;

public:
    // Get sound cues relative to the physical surfaces called
    UFUNCTION(BlueprintCallable, Category = "AGR Footsteps")
    void GetSurfaceReaction(TEnumAsByte<EPhysicalSurface> InputParam, USoundCue*& Sound, UNiagaraSystem*& Particle);

    UFUNCTION(BlueprintCallable, Category = "AGR Footsteps")
    void GetVelocityRange(float& MinRange, float& MaxRange) const;

    UFUNCTION(BlueprintCallable, Category = "AGR Footsteps")
    void GetFootStepSettings(USoundAttenuation*& AttenuationRef, USoundConcurrency*& ConcurrencyRef) const;
};