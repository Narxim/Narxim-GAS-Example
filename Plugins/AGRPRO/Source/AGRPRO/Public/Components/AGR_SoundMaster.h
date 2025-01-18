// Copyright Adam Grodzki All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NiagaraComponent.h"

#include "AGR_SoundMaster.generated.h"

class ACharacter;
class UDA_AGR_FootstepConfig;
class USoundCue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(
    FCharacterMadeFootstepSound,
    FHitResult,
    HitEvent,
    float,
    VolumeMulti,
    UAudioComponent*,
    Sound,
    UNiagaraSystem*,
    Particle,
    FName,
    Key);

UCLASS(Blueprintable, ClassGroup=("AGR"), meta=(BlueprintSpawnableComponent))
class AGRPRO_API UAGR_SoundMaster : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category = "AGR|Footstep")
    FCharacterMadeFootstepSound CharacterMadeFootstepSound;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AGR|Foot step Setup")
    float VolumeMultiplier = 1.0f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AGR|Foot step Setup")
    float SurfaceTraceLength = 5.0f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AGR|Foot step Setup")
    UDA_AGR_FootstepConfig* FootstepSetup;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AGR|Foot step Setup")
    TMap<FName, FName> FeetDefinitions = {{"L", "foot_l"}, {"R", "foot_r"}};

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AGR|Foot step AutoSetup")
    bool bAutoPlace;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AGR|Foot step AutoSetup")
    float MinTimeCycle = 0.25f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AGR|Foot step AutoSetup")
    bool bAutoVolume;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Internal")
    ACharacter* OwnerAsCharacter;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Internal")
    TArray<FName> AutoFootstepsArray;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Internal")
    FName PreviousFoot;

#if WITH_EDITORONLY_DATA
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AGR|Global Setup")
    bool bDebug = false;
#endif

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AGR|Global Setup")
    bool bTraceComplex = true;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "AGR|Global Setup")
    TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_Visibility;

private:
    FTimerHandle AutoFootStepGateTimerHandle;
    bool bAutoFootStepGateOpen = true;

public:
    // Sets default values for this component's properties
    UAGR_SoundMaster();

    virtual void TickComponent(
        const float DeltaTime,
        const ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    // Trace by foot name
    UFUNCTION(BlueprintCallable, Category = "AGR|Footstep")
    bool TryTraceFootstep(const FName Key);

    UFUNCTION(BlueprintCallable, Category = "AGR|Footstep")
    void FootstepEffect(const FHitResult& Hit, UAudioComponent*& Sound, UNiagaraSystem*& Particle);

    // Auto footstep placement
    UFUNCTION(BlueprintCallable, Category = "AGR|Footstep")
    void TestAllFeetForCollision();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AGR|Footstep")
    float OverwriteCalcVolume() const;
    virtual float OverwriteCalcVolume_Implementation() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AGR|Footstep")
    FVector OverwriteParticleScale() const;
    virtual FVector OverwriteParticleScale_Implementation() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AGR|Footstep")
    void OverwritePickSurface(
        const TEnumAsByte<EPhysicalSurface>& InSurface,
        TEnumAsByte<EPhysicalSurface>& OutSurface);
    virtual void OverwritePickSurface_Implementation(
        const TEnumAsByte<EPhysicalSurface>& InSurface,
        TEnumAsByte<EPhysicalSurface>& OutSurface);

private:
    void RecastOwner();

    void FootstepSoundEffect(USoundCue* SoundToPlay, const FVector& Location, UAudioComponent*& FootStepSound) const;
    void FootstepParticleEffect(
        UNiagaraSystem* ParticleToSpawn,
        const FVector& Location,
        const FVector& Normal,
        UNiagaraSystem*& NiagaraSystem) const;

    void OpenAutoFootStepGate();
    void CloseAutoFootStepGate();
};