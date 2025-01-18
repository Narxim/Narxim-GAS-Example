// Copyright 2021 Adam Grodzki All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/AGRAnimMasterComponent.h"
#include "GameFramework/Character.h"
#include "AGRCharacter.generated.h"

UCLASS(Abstract)
class AGRPRO_API AAGRCharacter : public ACharacter
{
    GENERATED_BODY()

private:
    UPROPERTY(Category = "AGR PRO", VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
    UAGRAnimMasterComponent* AGRAnimMasterComponent;

public:
    // Sets default values for this character's properties
    AAGRCharacter();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    void InitSkeletalMeshComponent() const;
    void InitCharacterMovementComponent() const;
    void InitAGRAnimMasterComponent();
};