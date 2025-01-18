// Copyright Adam Grodzki All Rights Reserved.

#include "Components/AGR_SoundMaster.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "Data/DA_AGR_FootstepConfig.h"
#include "Sound/SoundCue.h"

UAGR_SoundMaster::UAGR_SoundMaster()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UAGR_SoundMaster::BeginPlay()
{
    Super::BeginPlay();

    RecastOwner();
}

void UAGR_SoundMaster::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if(bAutoPlace)
    {
        TestAllFeetForCollision();
    }
}

void UAGR_SoundMaster::RecastOwner()
{
    if(!GetOwner())
    {
        return;
    }

    OwnerAsCharacter = Cast<ACharacter>(GetOwner());
}

void UAGR_SoundMaster::TestAllFeetForCollision()
{
    for(const auto& FeetDefinitionPair : FeetDefinitions)
    {
        const FName Key = FeetDefinitionPair.Key;
        const FName Value = FeetDefinitionPair.Value;
        FName LocalFeet = FeetDefinitions.FindRef(Key);

        const bool bContainsFeet = !(AutoFootstepsArray.Contains(LocalFeet) || LocalFeet == PreviousFoot);
        if(bContainsFeet)
        {
            const bool bSuccess = TryTraceFootstep(Key);
            if(bSuccess)
            {
                AutoFootstepsArray.AddUnique(LocalFeet);
                PreviousFoot = LocalFeet;
            }
        }
    }

    if(!bAutoFootStepGateOpen || AutoFootstepsArray.Num() < FeetDefinitions.Num())
    {
        return;
    }

    if(UKismetMathLibrary::NearlyEqual_FloatFloat(OwnerAsCharacter->GetVelocity().Size(), 0.0f, 0.01f))
    {
        return;
    }

    AutoFootstepsArray.Empty();
    CloseAutoFootStepGate();

    GetWorld()->GetTimerManager().SetTimer(
        AutoFootStepGateTimerHandle,
        this,
        &UAGR_SoundMaster::OpenAutoFootStepGate,
        MinTimeCycle,
        false);
}

void UAGR_SoundMaster::OpenAutoFootStepGate()
{
    bAutoFootStepGateOpen = true;
}

void UAGR_SoundMaster::CloseAutoFootStepGate()
{
    bAutoFootStepGateOpen = false;
}

bool UAGR_SoundMaster::TryTraceFootstep(const FName Key)
{
    if(!IsValid(OwnerAsCharacter))
    {
        RecastOwner();
        if(!ensure(IsValid(OwnerAsCharacter)))
        {
            return false;
        }
    }

    if(!FeetDefinitions.Contains(Key))
    {
#if WITH_EDITOR
        if(bDebug && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                15.0f,
                FColor::Red,
                FString::Printf(TEXT("FatalError! Foot name {1} not defined!")));
        }
#endif

        return false;
    }

    FName SocketName = FeetDefinitions.FindRef(Key);
    if(!OwnerAsCharacter->GetMesh()->DoesSocketExist(SocketName))
    {
#if WITH_EDITOR
        if(bDebug && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                15.0f,
                FColor::Red,
                FString::Printf(TEXT("FatalError! Bone/Socket name: {1} does not exist in parent skeleton!")));
        }
#endif

        return false;
    }

    const FVector Start = OwnerAsCharacter->GetMesh()->GetSocketLocation(SocketName);
    const FVector End = Start + FVector(0, 0, SurfaceTraceLength * -1);

    TArray<AActor*> IgnoredActors;
    OwnerAsCharacter->GetAttachedActors(IgnoredActors, true);

    TArray<AActor*> CachedArray;
    CachedArray.Add(OwnerAsCharacter);
    CachedArray.Append(IgnoredActors);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActors(CachedArray);
    QueryParams.AddIgnoredActor(GetOwner());
    QueryParams.bTraceComplex = bTraceComplex;
    QueryParams.bReturnPhysicalMaterial = true;
#if WITH_EDITOR
    QueryParams.bDebugQuery = bDebug;
#endif

    FHitResult HitResult;
    const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, TraceChannel, QueryParams);
    if(bHit)
    {
#if WITH_EDITOR
        if(bDebug)
        {
            DrawDebugSphere(
                GetWorld(),
                HitResult.ImpactPoint,
                25.0f,
                12,
                FColor::Blue,
                false,
                0.5f,
                0,
                1.0f);
        }
#endif

        // Make sound and call event dispatcher
        UAudioComponent* AudioComponent = nullptr;
        UNiagaraSystem* NiagaraSystem = nullptr;
        FootstepEffect(HitResult, AudioComponent, NiagaraSystem);
        CharacterMadeFootstepSound.Broadcast(HitResult, OverwriteCalcVolume(), AudioComponent, NiagaraSystem, Key);
    }

    return bHit;
}

void UAGR_SoundMaster::FootstepEffect(const FHitResult& Hit, UAudioComponent*& Sound, UNiagaraSystem*& Particle)
{
    if(!IsValid(FootstepSetup))
    {
#if WITH_EDITOR
        if(bDebug && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                15.0f,
                FColor::Red,
                FString::Printf(TEXT("Error: Variable 'FootstepSetup' in '%s' is unset"), *GetFName().ToString()));
        }
#endif

        return;
    }

    const FVector SpawnLocation = Hit.ImpactPoint;
    const FVector SpawnNormal = Hit.Normal;
    TEnumAsByte<EPhysicalSurface> SurfaceType = UGameplayStatics::GetSurfaceType(Hit);
    OverwritePickSurface(SurfaceType, SurfaceType);

    // Get needed data from Data Asset to spawn sound and particles
    USoundCue* SoundToPlay;
    UNiagaraSystem* ParticleToSpawn;
    FootstepSetup->GetSurfaceReaction(SurfaceType, SoundToPlay, ParticleToSpawn);

    FootstepSoundEffect(SoundToPlay, SpawnLocation, Sound);
    FootstepParticleEffect(ParticleToSpawn, SpawnLocation, SpawnNormal, Particle);
}

void UAGR_SoundMaster::FootstepSoundEffect(
    USoundCue* SoundToPlay,
    const FVector& Location,
    UAudioComponent*& FootStepSound) const
{
    if(!IsValid(SoundToPlay))
    {
        return;
    }

    USoundConcurrency* Concurrency;
    USoundAttenuation* Attenuation;
    FootstepSetup->GetFootStepSettings(Attenuation, Concurrency);

    FootStepSound = UGameplayStatics::SpawnSoundAtLocation(
        GetWorld(),
        SoundToPlay,
        Location,
        FRotator(0, 0, 0),
        bAutoVolume ? OverwriteCalcVolume() : VolumeMultiplier,
        1.0f,
        0.0f,
        Attenuation,
        Concurrency,
        true);
}

void UAGR_SoundMaster::FootstepParticleEffect(
    UNiagaraSystem* ParticleToSpawn,
    const FVector& Location,
    const FVector& Normal,
    UNiagaraSystem*& NiagaraSystem) const
{
    if(!IsValid(ParticleToSpawn))
    {
        return;
    }

    const FRotator SpawnRotation = FRotationMatrix::MakeFromZ(Normal).Rotator();
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        GetWorld(),
        ParticleToSpawn,
        Location,
        SpawnRotation,
        OverwriteParticleScale(),
        true,
        true,
        ENCPoolMethod::None,
        true);
    NiagaraSystem = ParticleToSpawn;
}

float UAGR_SoundMaster::OverwriteCalcVolume_Implementation() const
{
    if(!IsValid(FootstepSetup))
    {
#if WITH_EDITOR
        if(bDebug && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                15.0f,
                FColor::Red,
                FString::Printf(TEXT("FatalError! DA is EMPTY !!")));
        }
#endif

        return 0.0f;
    }

    float VelocityMin;
    float VelocityMax;
    FootstepSetup->GetVelocityRange(VelocityMin, VelocityMax);

    const float ValueToClampOn = OwnerAsCharacter->GetVelocity().Size();
    const FVector2D InputRange = FVector2D(VelocityMin, VelocityMax);
    const FVector2D OutputRange = FVector2D(0.2f, 1.0f);
    const float CalculatedVolume = FMath::GetMappedRangeValueClamped(InputRange, OutputRange, ValueToClampOn) *
                                   VolumeMultiplier;
    return CalculatedVolume;
}

FVector UAGR_SoundMaster::OverwriteParticleScale_Implementation() const
{
    return GetOwner()->GetActorScale3D();
}

void UAGR_SoundMaster::OverwritePickSurface_Implementation(
    const TEnumAsByte<EPhysicalSurface>& InSurface,
    TEnumAsByte<EPhysicalSurface>& OutSurface)
{
    OutSurface = InSurface;
}