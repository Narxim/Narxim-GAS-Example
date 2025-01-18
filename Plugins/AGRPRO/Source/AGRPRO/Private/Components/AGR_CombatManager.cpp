// Copyright Adam Grodzki All Rights Reserved.

#include "Components/AGR_CombatManager.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UAGR_CombatManager::UAGR_CombatManager()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UAGR_CombatManager::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if(bTracing)
    {
        TraceTick();
    }
}

void UAGR_CombatManager::StartTrace()
{
    RefreshIgnoredActors();

    bTracing = true;

    // Entry sockets location
    PreviousStarts.SetNum(TrackedMeshes.Num());
    PreviousEnds.SetNum(TrackedMeshes.Num());
    int32 Index = 0;
    for(const auto& Entry : TrackedMeshes)
    {
        UPrimitiveComponent* Mesh = Entry.Key;
        if(!IsValid(Mesh))
        {
            RemoveTraceMesh(Mesh);
            continue;
        }
        // const EAGR_CombatColliderType CollisionHandling = Entry.Value;

        FVector WorldStart;
        FVector WorldEnd;
        GetStartEndLocationByMesh(Mesh, WorldStart, WorldEnd);
        PreviousStarts[Index] = WorldStart;
        PreviousEnds[Index] = WorldEnd;

        ++Index;
    }

    OnStartAttack.Broadcast();
    TraceTick();
}

void UAGR_CombatManager::EndTrace()
{
    bTracing = false;

    // Clear unimportant variables
    PreviousStarts.Empty();
    PreviousEnds.Empty();

    OnEndAttack.Broadcast();
}

void UAGR_CombatManager::SetupDebug(const bool bInDebug)
{
    bDebug = bInDebug;
}

void UAGR_CombatManager::AddTraceMesh(
    UPrimitiveComponent* Mesh,
    const EAGR_CombatColliderType CollisionHandling = EAGR_CombatColliderType::SimpleCapsuleTrace)
{
    if(!IsValid(Mesh))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid mesh passed, Nothing will be added to TrackedMeshes"));
        return;
    }
    TrackedMeshes.Add(Mesh, CollisionHandling);
}

void UAGR_CombatManager::RemoveTraceMesh(UPrimitiveComponent* Mesh)
{
    if(!TrackedMeshes.Find(Mesh))
    {
        return;
    }

    TrackedMeshes.Remove(Mesh);
}

void UAGR_CombatManager::ClearAllMeshes()
{
    TrackedMeshes.Empty();
}

void UAGR_CombatManager::RefreshIgnoredActors()
{
    GetOwner()->GetAttachedActors(IgnoredActors);
}

void UAGR_CombatManager::GetAllWeapons(TArray<AActor*>& OutWeapons)
{
    for(const auto& TrackedMesh : TrackedMeshes)
    {
        OutWeapons.Add(TrackedMesh.Key->GetOwner());
    }
}

void UAGR_CombatManager::GetStartEndLocationByMesh(
    const UPrimitiveComponent* Mesh,
    FVector& OutWorldStart,
    FVector& OutWorldEnd) const
{
    OutWorldStart = Mesh->GetSocketLocation(StartSocketTraceName);
    OutWorldEnd = Mesh->GetSocketLocation(EndSocketTraceName);
}

// Called on each frame that traces combat
void UAGR_CombatManager::TraceTick()
{
    // For each weapon do as in config
    int32 Index = 0;
    for(const auto& Entry : TrackedMeshes)
    {
        UPrimitiveComponent* Mesh = Entry.Key;
        const EAGR_CombatColliderType CollisionHandling = Entry.Value;

        // Config selector
        switch(CollisionHandling)
        {
        case EAGR_CombatColliderType::SimpleLineTrace:
            {
                DoCollisionTest_SimpleLineTrace(Mesh, TraceColor1);
                break;
            }

        case EAGR_CombatColliderType::SimpleBoxTrace:
            {
                DoCollisionTest_SimpleBoxTrace(Mesh, TraceColor1);
                break;
            }

        case EAGR_CombatColliderType::SimpleCapsuleTrace:
            {
                DoCollisionTest_SimpleCapsuleTrace(Mesh, TraceColor1);
                break;
            }

        case EAGR_CombatColliderType::ComplexLineTrace:
            {
                DoCollisionTest_ComplexLineTrace(Index, Mesh, TraceColor1, TraceColor2);
                break;
            }

        case EAGR_CombatColliderType::ComplexBoxTrace:
            {
                DoCollisionTest_ComplexBoxTrace(Index, Mesh, TraceColor1);
                break;
            }

        default:
            {
                checkNoEntry();
            }
        }

        ++Index;
    }
}

void UAGR_CombatManager::GetPreviousPoints(const int32 Index, FVector& OutStart, FVector& OutEnd) const
{
    if(Index < 0 || Index >= PreviousStarts.Num() || Index >= PreviousEnds.Num())
    {
        return;
    }

    OutStart = PreviousStarts[Index];
    OutEnd = PreviousEnds[Index];
}

void UAGR_CombatManager::CalculateTracePoint(
    const int32 Index,
    const FVector& Start,
    const FVector& End,
    FVector& OutPoint) const
{
    OutPoint = Start + Index * ((End - Start) / TracePrecision);
}

void UAGR_CombatManager::HandleOnAttackHitEvent(
    const bool bHit,
    const TArray<FHitResult>& HitResults,
    UPrimitiveComponent* Mesh) const
{
    if(!bHit)
    {
        return;
    }

    for(const auto& HitResult : HitResults)
    {
        OnAttackHitEvent.Broadcast(HitResult, Mesh);
    }
}

void UAGR_CombatManager::DoCollisionTest_SimpleLineTrace(UPrimitiveComponent* Mesh, const FColor DebugTraceColor) const
{
    TArray<FHitResult> HitResults;

    FVector Start;
    FVector End;
    GetStartEndLocationByMesh(Mesh, Start, End);

    const bool bHit = UKismetSystemLibrary::LineTraceMulti(
        this,
        Start,
        End,
        TraceChannel,
        bTraceComplex,
        IgnoredActors,
        bDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
        HitResults,
        true,
        DebugTraceColor);

    HandleOnAttackHitEvent(bHit, HitResults, Mesh);
}

void UAGR_CombatManager::DoCollisionTest_SimpleBoxTrace(UPrimitiveComponent* Mesh, const FColor DebugTraceColor) const
{
    TArray<FHitResult> HitResults;

    FVector Start;
    FVector End;
    GetStartEndLocationByMesh(Mesh, Start, End);

    const FVector HalfSize = FVector(TraceSize);
    const FRotator Orientation = UKismetMathLibrary::FindLookAtRotation(Start, End);
    const bool bHit = UKismetSystemLibrary::BoxTraceMulti(
        this,
        Start,
        End,
        HalfSize,
        Orientation,
        TraceChannel,
        bTraceComplex,
        IgnoredActors,
        bDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
        HitResults,
        true,
        DebugTraceColor);

    HandleOnAttackHitEvent(bHit, HitResults, Mesh);
}

void UAGR_CombatManager::DoCollisionTest_SimpleCapsuleTrace(
    UPrimitiveComponent* Mesh,
    const FColor DebugTraceColor) const
{
    TArray<FHitResult> HitResults;

    FVector Start;
    FVector End;
    GetStartEndLocationByMesh(Mesh, Start, End);

    const bool bHit = UKismetSystemLibrary::SphereTraceMulti(
        this,
        Start,
        End,
        TraceSize,
        TraceChannel,
        bTraceComplex,
        IgnoredActors,
        bDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
        HitResults,
        true,
        DebugTraceColor);

    HandleOnAttackHitEvent(bHit, HitResults, Mesh);
}

void UAGR_CombatManager::DoCollisionTest_ComplexLineTrace(
    const int32 TrackedMeshIndex,
    UPrimitiveComponent* Mesh,
    const FColor DebugTraceColor1,
    const FColor DebugTraceColor2)
{
    DoCollisionTest_SimpleLineTrace(Mesh, DebugTraceColor2);

    for(int32 Index = 0; Index <= TracePrecision; ++Index)
    {
        TArray<FHitResult> HitResults;

        // Points in between calculation
        FVector TempStart;
        FVector TempEnd;

        FVector Start;
        GetPreviousPoints(TrackedMeshIndex, TempStart, TempEnd);
        CalculateTracePoint(Index, TempStart, TempEnd, Start);

        FVector End;
        GetStartEndLocationByMesh(Mesh, TempStart, TempEnd);
        CalculateTracePoint(Index, TempStart, TempEnd, End);

        const bool bHit = UKismetSystemLibrary::LineTraceMulti(
            this,
            Start,
            End,
            TraceChannel,
            bTraceComplex,
            IgnoredActors,
            bDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
            HitResults,
            true,
            DebugTraceColor1);

        HandleOnAttackHitEvent(bHit, HitResults, Mesh);
    }

    // Set next frame targets
    PreviousStarts.SetNum(TrackedMeshes.Num());
    PreviousEnds.SetNum(TrackedMeshes.Num());

    FVector Start;
    FVector End;
    GetStartEndLocationByMesh(Mesh, Start, End);
    PreviousStarts[TrackedMeshIndex] = Start;
    PreviousEnds[TrackedMeshIndex] = End;
}

void UAGR_CombatManager::DoCollisionTest_ComplexBoxTrace(
    const int32 TrackedMeshIndex,
    UPrimitiveComponent* Mesh,
    const FColor DebugTraceColor)
{
    for(int32 Index = 0; Index <= TracePrecision; ++Index)
    {
        TArray<FHitResult> HitResults;

        // Points in between calculation
        FVector TempStart;
        FVector TempEnd;

        FVector Start;
        GetPreviousPoints(TrackedMeshIndex, TempStart, TempEnd);
        CalculateTracePoint(Index, TempStart, TempEnd, Start);

        FVector End;
        GetStartEndLocationByMesh(Mesh, TempStart, TempEnd);
        CalculateTracePoint(Index, TempStart, TempEnd, End);

        const FVector HalfSize = FVector(TraceSize);
        const FRotator Orientation = UKismetMathLibrary::FindLookAtRotation(Start, End);
        const bool bHit = UKismetSystemLibrary::BoxTraceMulti(
            this,
            Start,
            End,
            HalfSize,
            Orientation,
            TraceChannel,
            bTraceComplex,
            IgnoredActors,
            bDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
            HitResults,
            true,
            DebugTraceColor);

        HandleOnAttackHitEvent(bHit, HitResults, Mesh);
    }

    // Set next frame targets
    PreviousStarts.SetNum(TrackedMeshes.Num());
    PreviousEnds.SetNum(TrackedMeshes.Num());

    FVector Start;
    FVector End;
    GetStartEndLocationByMesh(Mesh, Start, End);
    PreviousStarts[TrackedMeshIndex] = Start;
    PreviousEnds[TrackedMeshIndex] = End;
}