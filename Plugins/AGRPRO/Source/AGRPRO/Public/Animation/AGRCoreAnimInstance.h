// Copyright 2021 Adam Grodzki All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Components/AGRAnimMasterComponent.h"
#include "Data/AGRTypes.h"
#include "AGRCoreAnimInstance.generated.h"

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class AGRPRO_API UAGRCoreAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "AGR|Components")
    ACharacter* OwningCharacter;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Components")
    UCharacterMovementComponent* OwnerMovementComponent;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Components")
    UAGRAnimMasterComponent* AnimMasterComponent;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Anim States")
    FGameplayTagContainer ModificationTags;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Anim States")
    FGameplayTag BasePose;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Anim States")
    FGameplayTag OverlayPose;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|AimOffset")
    FRotator RawAimOffset;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|AimOffset")
    EAGR_AimOffsets AimOffsetType;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|AimOffset")
    EAGR_AimOffsetClamp AimOffsetBehavior;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|AimOffset")
    FVector LookAtLocation;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|AimOffset")
    FVector2D FinalAimOffset;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|AimOffset")
    float AimClamp;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|AimOffset")
    float AimPitch;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Rotation")
    EAGR_RotationMethod RotationMethod;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Runtime")
    float DeltaTick;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Runtime")
    FRotator PreviousRotation;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Movement")
    FRotator PreviousFrameAim;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Movement")
    float Velocity;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Movement")
    float ForwardVelocity;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Movement")
    float StrafeVelocity;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Movement")
    float UpVelocity;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Movement")
    float Direction;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Runtime")
    FVector InputAcceleration;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|State")
    bool bFirstPerson;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|State")
    bool bIdle;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|State")
    bool bInAir;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|State")
    bool bStanding;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|State")
    bool bSwimming;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|State")
    bool bCrouching;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|State")
    bool bGrounded;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|State")
    bool bWalkingState;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|State")
    bool bFlying;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|State")
    bool bFalling;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|State")
    TEnumAsByte<EMovementMode> MovementMode;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Leans")
    float Lean;

    UPROPERTY(BlueprintReadOnly, Category = "AGR|Leans")
    FVector2D AimDelta;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "AGR|Setup")
    float TargetFrameRate;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "AGR|Setup")
    float LeanSmooth;

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "AGR|Setup")
    float AimSmooth;

private:
    UPROPERTY()
    APawn* PawnReference;

public:
    UAGRCoreAnimInstance(const FObjectInitializer& ObjectInitializer);

private:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    void RecastOwnerComponents();
    void GetComponentVariables();
    void SetMovementVectorsAndStates();
    void SetupLeaning();
    void SetupAimOffset();
    void SetupMovementStates();
    float NormalizeLean(const float InValue) const;
    bool IsStanding() const;
};