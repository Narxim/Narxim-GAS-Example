// Copyright 2021 Adam Grodzki All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "AGRTypes.generated.h"

UENUM(BlueprintType)
enum class EAGR_AimOffsetClamp : uint8
{
    Nearest = 0 UMETA(DisplayName = "Nearest"),
    Left UMETA(DisplayName = "Left"),
    Right UMETA(DisplayName = "Right")
};

UENUM(BlueprintType)
enum class EAGR_AimOffsets:uint8
{
    NONE = 0 UMETA(DisplayName = "NONE"),
    Aim UMETA(DisplayName = "Aim"),
    Look UMETA(DisplayName = "Look")
};

UENUM(BlueprintType)
enum class EAGR_RotationMethod : uint8
{
    NONE = 0 UMETA(DisplayName = "NONE"),
    RotateToVelocity UMETA(DisplayName = "Rotate To Velocity"),
    AbsoluteRotation UMETA(DisplayName = "Absolute Rotation"),
    DesiredRotation UMETA(DisplayName = "Desired Rotation"),
    DesiredAtAngle UMETA(DisplayName = "Desired At Angle")
};

USTRUCT(BlueprintType)
struct FEquipment
{
    GENERATED_BODY()
    ;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="AGR")
    FName Id;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="AGR")
    FGameplayTagContainer AcceptableSlots;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="AGR")
    AActor* ItemActor = nullptr;
};

/**
 * Types of tracing for objects
 */
UENUM(BlueprintType)
enum class EAGR_CombatColliderType : uint8
{
    // Simplest line trace avaiable. usable for mass adaptation without impacting performance
    SimpleLineTrace = 0 UMETA(DisplayName = "Simple Line Trace"),
    // Simple collision sweep with box instead of lines for additional thickness
    SimpleBoxTrace UMETA(DisplayName = "Simple Box Trace"),
    // Simple trace with capsule along the weapon
    SimpleCapsuleTrace UMETA(DisplayName = "Simple Capsule Trace"),
    // Horizontal line tracing
    ComplexLineTrace UMETA(DisplayName = "Complex Line Trace"),
    // Horizontal box traces
    ComplexBoxTrace UMETA(DisplayName = "Complex Box Trace"),

    //~ TODO Not implemented yet
    // Most expensive but most precise for super fast motion or low fps. works on 2 fps.
    // ProceduralMeshARC	UMETA(DisplayName = "Procedural Mesh Arc"),
};