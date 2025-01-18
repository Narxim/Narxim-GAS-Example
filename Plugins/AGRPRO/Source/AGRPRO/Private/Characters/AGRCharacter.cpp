// Copyright 2021 Adam Grodzki All Rights Reserved.

#include "Characters/AGRCharacter.h"

// =============================================================================
// These includes are needed to prevent plugin build failures.
#include "Components/SkeletalMeshComponent.h"
// =============================================================================

// Sets default values
AAGRCharacter::AAGRCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    InitSkeletalMeshComponent();
    InitCharacterMovementComponent();
    InitAGRAnimMasterComponent();

    bUseControllerRotationYaw = false;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    NetPriority = 4.0f;
}

// Called when the game starts or when spawned
void AAGRCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AAGRCharacter::InitSkeletalMeshComponent() const
{
    USkeletalMeshComponent* MeshComponent = GetMesh();

    // MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
    // MeshComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    MeshComponent->bEnableUpdateRateOptimizations = true;
    MeshComponent->bPropagateCurvesToFollowers = true;

    // This is important when running simulating this character on a dedicated server so it doesn't stay in a T-Pose only.
    MeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}

void AAGRCharacter::InitCharacterMovementComponent() const
{
    UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();

    // General
    CharacterMovementComponent->PerchRadiusThreshold = 40.0f;
    CharacterMovementComponent->PerchAdditionalHeight = 25.0f;

    // Flying
    CharacterMovementComponent->NavAgentProps.bCanFly = true;

    // Crouching
    CharacterMovementComponent->NavAgentProps.bCanCrouch = true;
    CharacterMovementComponent->bCanWalkOffLedgesWhenCrouching = true;
}

void AAGRCharacter::InitAGRAnimMasterComponent()
{
    AGRAnimMasterComponent = CreateDefaultSubobject<UAGRAnimMasterComponent>("AGRAnimMaster");
}

// Called every frame
void AAGRCharacter::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AAGRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}