// Copyright 2021 Adam Grodzki All Rights Reserved.

#include "Animation/AGRCoreAnimInstance.h"

#include "Data/AGRLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"

UAGRCoreAnimInstance::UAGRCoreAnimInstance(const FObjectInitializer& ObjectInitializer)
    : UAnimInstance(ObjectInitializer)
{
    TargetFrameRate = 60.0f;
    LeanSmooth = 6.0f;
    AimSmooth = 6.0f;
}

void UAGRCoreAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    RecastOwnerComponents();
}

void UAGRCoreAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    DeltaTick = DeltaSeconds;

    PawnReference = TryGetPawnOwner();
    if(!IsValid(PawnReference))
    {
        return;
    }

    // All functions below this comment expect a valid pawn reference
    GetComponentVariables();
    SetMovementVectorsAndStates();
    SetupLeaning();
    SetupAimOffset();
    SetupMovementStates();
}

void UAGRCoreAnimInstance::RecastOwnerComponents()
{
    OwningCharacter = Cast<ACharacter>(PawnReference);
    if(OwningCharacter)
    {
        OwnerMovementComponent = OwningCharacter->GetCharacterMovement();
        AnimMasterComponent = UAGRLibrary::GetAnimationMaster(OwningCharacter);
    }
}

void UAGRCoreAnimInstance::GetComponentVariables()
{
    if(AnimMasterComponent)
    {
        ModificationTags = AnimMasterComponent->AnimModTags;
        BasePose = AnimMasterComponent->BasePose;
        OverlayPose = AnimMasterComponent->OverlayPose;
        AimOffsetType = AnimMasterComponent->AimOffsetType;
        AimOffsetBehavior = AnimMasterComponent->AimOffsetBehavior;
        RawAimOffset = AnimMasterComponent->AimOffset;
        RotationMethod = AnimMasterComponent->RotationMethod;
        bFirstPerson = AnimMasterComponent->bFirstPerson;
        LookAtLocation = AnimMasterComponent->LookAtLocation;
        AimClamp = AnimMasterComponent->AimClamp;
    }
    else
    {
        RecastOwnerComponents();
    }
}

void UAGRCoreAnimInstance::SetMovementVectorsAndStates()
{
    const FVector VelocityVector = PawnReference->GetVelocity();
    Velocity = VelocityVector.Size();
    const FRotator Rotation = PawnReference->GetActorRotation();

    const FVector UnRotatedVelocityVector = Rotation.UnrotateVector(VelocityVector);

    ForwardVelocity = UnRotatedVelocityVector.X;
    StrafeVelocity = UnRotatedVelocityVector.Y;
    UpVelocity = UnRotatedVelocityVector.Z;

    const float IdleXY = FVector(VelocityVector.X, VelocityVector.Y, 0.0f).Size();

    bIdle = FMath::IsNearlyZero(IdleXY);
    if(!bIdle)
    {
            Direction = UKismetAnimationLibrary::CalculateDirection(PawnReference->GetVelocity(), PawnReference->GetActorRotation());
    }
    if(OwnerMovementComponent)
    {
        InputAcceleration = OwnerMovementComponent->GetCurrentAcceleration();
    }
}

void UAGRCoreAnimInstance::SetupLeaning()
{
    const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(
        PreviousRotation,
        PawnReference->GetActorRotation());
    Lean = FMath::FInterpTo(Lean, NormalizeLean(Delta.Yaw), DeltaTick, LeanSmooth);
    PreviousRotation = PawnReference->GetActorRotation();
}

void UAGRCoreAnimInstance::SetupAimOffset()
{
    FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(RawAimOffset, PawnReference->GetActorRotation());
    const float Min = AimClamp * -1.0f;
    const float Max = AimClamp;

    float TargetX = TargetX = Delta.Yaw;
    const float TargetY = Delta.Pitch;

    if(!UKismetMathLibrary::InRange_FloatFloat(Delta.Yaw, Min, Max, true, true))
    {
        switch(AimOffsetBehavior)
        {
        case EAGR_AimOffsetClamp::Nearest:
            TargetX = FMath::Clamp(Delta.Yaw, Min, Max);
            break;

        case EAGR_AimOffsetClamp::Left:
            TargetX = Min;
            break;

        case EAGR_AimOffsetClamp::Right:
            TargetX = Max;
            break;

        default:
            checkNoEntry();
        }
    }

    AimPitch = UKismetMathLibrary::NormalizedDeltaRotator(
        PawnReference->GetBaseAimRotation(),
        PawnReference->GetActorRotation()).Pitch;

    FinalAimOffset = FMath::Vector2DInterpTo(FinalAimOffset, FVector2D(TargetX, TargetY), DeltaTick, AimSmooth);

    Delta = UKismetMathLibrary::NormalizedDeltaRotator(PreviousFrameAim, RawAimOffset);
    AimDelta = FMath::Vector2DInterpTo(
        AimDelta,
        FVector2D(NormalizeLean(Delta.Yaw), NormalizeLean(Delta.Pitch)),
        DeltaTick,
        LeanSmooth);
    PreviousFrameAim = RawAimOffset;
}

void UAGRCoreAnimInstance::SetupMovementStates()
{
    if(OwnerMovementComponent)
    {
        bFalling = OwnerMovementComponent->IsFalling();
        bCrouching = OwnerMovementComponent->IsCrouching();
        bFlying = OwnerMovementComponent->IsFlying();
        bSwimming = OwnerMovementComponent->IsSwimming();
        bWalkingState = OwnerMovementComponent->IsWalking();
        bGrounded = OwnerMovementComponent->IsMovingOnGround();
        bInAir = bFalling || bFlying;
        MovementMode = OwnerMovementComponent->MovementMode;
    }

    bStanding = IsStanding();
}

float UAGRCoreAnimInstance::NormalizeLean(const float InValue) const
{
    return (InValue * (1.0 / DeltaTick)) / TargetFrameRate;
}

bool UAGRCoreAnimInstance::IsStanding() const
{
    return !bCrouching;
}