// Copyright Adam Grodzki All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/AGRTypes.h"

#include "AGR_CombatManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttackHitEvent, FHitResult, Hit, UPrimitiveComponent*, Mesh);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartAttack);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndAttack);

UCLASS(BlueprintType, Blueprintable, ClassGroup=("AGR"), meta=(BlueprintSpawnableComponent))
class AGRPRO_API UAGR_CombatManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AGR|SETUP")
    FName StartSocketTraceName = "Start";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AGR|SETUP")
    FName EndSocketTraceName = "End";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AGR|SETUP")
    TEnumAsByte<ETraceTypeQuery> TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AGR|SETUP")
    bool bTraceComplex = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AGR|SETUP")
    int32 TracePrecision = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AGR|SETUP")
    float TraceSize = 5.0f;

    UPROPERTY(BlueprintAssignable, Category = "AGR|Events")
    FOnAttackHitEvent OnAttackHitEvent;

    UPROPERTY(BlueprintAssignable, Category = "AGR|Events")
    FOnStartAttack OnStartAttack;

    UPROPERTY(BlueprintAssignable, Category = "AGR|Events")
    FOnEndAttack OnEndAttack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AGR|Debug")
    bool bDebug = false;

    UPROPERTY(EditDefaultsOnly, Category = "AGR|Debug")
    FColor TraceColor1 = FColor::Red;

    UPROPERTY(EditDefaultsOnly, Category = "AGR|Debug")
    FColor TraceColor2 = FColor::Blue;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AGR|CollisionHandler")
    TMap<UPrimitiveComponent*, EAGR_CombatColliderType> TrackedMeshes;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AGR|CollisionHandler")
    TArray<AActor*> IgnoredActors;

private:
    bool bTracing = false;

    UPROPERTY()
    TArray<FVector> PreviousStarts;

    UPROPERTY()
    TArray<FVector> PreviousEnds;

public:
    UAGR_CombatManager();

    virtual void TickComponent(
        const float DeltaTime,
        const ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="AGR")
    void StartTrace();

    UFUNCTION(BlueprintCallable, Category="AGR")
    void EndTrace();

    UFUNCTION(BlueprintCallable, Category="AGR")
    void SetupDebug(UPARAM(DisplayName="Debug") const bool bInDebug);

    UFUNCTION(BlueprintCallable, Category="AGR")
    void AddTraceMesh(UPrimitiveComponent* Mesh, const EAGR_CombatColliderType CollisionHandling);

    UFUNCTION(BlueprintCallable, Category="AGR")
    void RemoveTraceMesh(UPrimitiveComponent* Mesh);

    UFUNCTION(BlueprintCallable, Category="AGR")
    void ClearAllMeshes();

    UFUNCTION(BlueprintCallable, Category="AGR")
    void RefreshIgnoredActors();

    UFUNCTION(BlueprintPure, Category="AGR")
    void GetAllWeapons(UPARAM(DisplayName="Weapons") TArray<AActor*>& OutWeapons);

private:
    void GetStartEndLocationByMesh(
        const UPrimitiveComponent* Mesh,
        UPARAM(DisplayName = "WorldStart") FVector& OutWorldStart,
        UPARAM(DisplayName = "WorldEnd") FVector& OutWorldEnd) const;
    void TraceTick();
    void GetPreviousPoints(
        const int32 Index,
        UPARAM(DisplayName = "Start") FVector& OutStart,
        UPARAM(DisplayName = "End") FVector& OutEnd) const;
    void CalculateTracePoint(
        const int32 Index,
        const FVector& Start,
        const FVector& End,
        UPARAM(DisplayName = "Point") FVector& OutPoint) const;
    void HandleOnAttackHitEvent(const bool bHit, const TArray<FHitResult>& HitResults, UPrimitiveComponent* Mesh) const;
    void DoCollisionTest_SimpleLineTrace(UPrimitiveComponent* Mesh, const FColor DebugTraceColor) const;
    void DoCollisionTest_SimpleBoxTrace(UPrimitiveComponent* Mesh, const FColor DebugTraceColor) const;
    void DoCollisionTest_SimpleCapsuleTrace(UPrimitiveComponent* Mesh, const FColor DebugTraceColor) const;
    void DoCollisionTest_ComplexLineTrace(
        const int32 TrackedMeshIndex,
        UPrimitiveComponent* Mesh,
        const FColor DebugTraceColor1,
        const FColor DebugTraceColor2);
    void DoCollisionTest_ComplexBoxTrace(
        const int32 TrackedMeshIndex,
        UPrimitiveComponent* Mesh,
        const FColor DebugTraceColor);
};