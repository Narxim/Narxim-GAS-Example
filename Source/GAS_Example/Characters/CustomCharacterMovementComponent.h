// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomCharacterMovementComponent.generated.h"


class ACharacterBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAS_EXAMPLE_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend ACharacterBase;
	
public:
	// Sets default values for this component's properties
	UCustomCharacterMovementComponent();

	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;
	virtual void BeginPlay() override;

protected:
	float MovementSpeedMultiplier = 1.f;
};
