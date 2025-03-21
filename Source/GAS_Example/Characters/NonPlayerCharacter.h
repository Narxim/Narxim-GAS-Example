// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "NonPlayerCharacter.generated.h"

// Non Player Character is the base class to use for Server controlled Characters.
// This class contains its own Ability System Component.
UCLASS(Blueprintable)
class GAS_EXAMPLE_API ANonPlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	ANonPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
};