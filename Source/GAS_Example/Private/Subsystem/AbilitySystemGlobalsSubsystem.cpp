// Copyright 2021 Joseph "Narxim" Thigpen.


#include "Subsystem/AbilitySystemGlobalsSubsystem.h"
#include "AbilitySystemGlobals.h"

void UAbilitySystemGlobalsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UAbilitySystemGlobals::Get().InitGlobalData();
}