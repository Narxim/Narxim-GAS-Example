// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "CustomEngineSubsystem.generated.h"

// Initializes the UAbilitySystemGlobals module. 

UCLASS()
class GAS_EXAMPLE_API UCustomEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};