// Copyright 2021 Joseph "Narxim" Thigpen.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "AbilitySystemGlobalsSubsystem.generated.h"

// Initializes the UAbilitySystemGlobals module.
// "UAbilitySystemGlobals::Get().InitGlobalData()" needs to be called to enable Ability Target Data.
// This Subsystem will automatically start up when the main game module is launched.
// Note: You could call this in other places. It is added here in the example for visibility.
UCLASS()
class GAS_EXAMPLE_API UAbilitySystemGlobalsSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};